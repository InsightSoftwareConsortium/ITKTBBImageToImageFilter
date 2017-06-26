#ifndef __itkTBBImageToImageFilter_hxx
#define __itkTBBImageToImageFilter_hxx

#include "itkTBBImageToImageFilter.h"

#include "itkImageSource.h"
#include "itkImageRegionSplitterBase.h"
#include "itkOutputDataObjectIterator.h"

#define JOB_PER_THREAD_RATIO 20

#ifdef ITK_USE_TBB
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/compat/thread>
#include <tbb/tick_count.h>
#endif // ITK_USE_TBB

namespace itk {

#ifdef ITK_USE_TBB
/**
 * \class    TBBFunctor
 *
 * \brief    TBB functor to execute jobs in parallel.
 *
 * \author    Amir Jaberzadeh, Benoit Scherrer and Etienne St-Onge
 *
 * \tparam    TInputImage     Type of the input image.
 * \tparam    TOutputImage    Type of the output image.
 **/
template< typename TInputImage, typename TOutputImage >
class TBBFunctor
{
public:
    typedef TBBFunctor                              Self;
    typedef TOutputImage                            OutputImageType;
    typedef typename OutputImageType::ConstPointer  OutputImageConstPointer;
    typedef typename TOutputImage::SizeType         OutputImageSizeType;
    typedef typename OutputImageType::RegionType    OutputImageRegionType;

    itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

    typedef TBBImageToImageFilter<TInputImage,TOutputImage> TbbImageFilterType;

    TBBFunctor(TbbImageFilterType *tbbFilter, const OutputImageSizeType& outputSize):
        m_TBBFilter(tbbFilter), m_OutputSize(outputSize) {}

    void operator() ( const tbb::blocked_range<int>& r ) const
    {
        typename TOutputImage::SizeType size = m_OutputSize;
        typename TOutputImage::IndexType index;
        index.Fill(0);

        if (m_TBBFilter->GetNbReduceDimensions() > 0)
        {
            unsigned int i = OutputImageDimension - (unsigned int)m_TBBFilter->GetNbReduceDimensions();

            index[i] = r.begin();
            size[i] = 1;
            while (i < OutputImageDimension - 1)
            {
                index[i+1] = index[i] / m_OutputSize[i];
                index[i] = index[i] % m_OutputSize[i];
                size[i+1] = 1;
                i++;
            }
        }

        // Construct an itk::ImageRegion
        OutputImageRegionType myRegion(index, size);

        // Run the TBBGenerateData method! (equivalent of ThreadedGenerateData)
        m_TBBFilter->TBBGenerateData(myRegion);
    }

private:
    TbbImageFilterType *m_TBBFilter;
    OutputImageSizeType m_OutputSize;
};
#endif // ITK_USE_TBB

// Constructor
template< typename TInputImage, typename TOutputImage >
TBBImageToImageFilter< TInputImage, TOutputImage >::TBBImageToImageFilter()
{
    // By default, Automatic NbReduceDimensions
    this->SetNbReduceDimensions(-1);

    // We d'ont need itk::barrier, itk::MultiThreader::SingleMethodeExecute
    // is already taking care of that job, (using itk::MultiThreader::WaitForSingleMethodThread)

#ifdef ITK_USE_TBB
    // By default, do not define the number of threads.
    // Let TBB doing that.
    this->SetNumberOfThreads(0);
#else

#endif // ITK_USE_TBB
}

// Destructor
template< typename TInputImage, typename TOutputImage >
TBBImageToImageFilter< TInputImage, TOutputImage >::~TBBImageToImageFilter()
{
}

/**
 * \fn    template< typename TInputImage, typename TOutputImage > void TBBImageToImageFilter< TInputImage, TOutputImage > ::GenerateData()
 *
 * \brief    New default implementation for GenerateData() to use TBB
 *
 * \author  Amir Jaberzadeh, Benoit Scherrer and Etienne St-Onge
 *
 * \tparam    typename TInputImage     Type of the typename t input image.
 * \tparam    typename TOutputImage    Type of the typename t output image.
 **/
template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::GenerateData()
{
    // Get the size of the requested region
    typename TOutputImage::ConstPointer output = static_cast<TOutputImage *>(this->ProcessObject::GetOutput(0));

    // Call a method that can be overriden by a subclass to allocate
    // memory for the filter's outputs
    this->AllocateOutputs();

    // Call a method that can be overridden by a subclass to perform
    // some calculations prior to splitting the main computations into
    // separate threads
    this->BeforeThreadedGenerateData();

#ifdef ITK_USE_TBB
    typename TOutputImage::SizeType outputSize = output->GetRequestedRegion().GetSize();

    // Set up the number of threads with default
    // if it was not previously set
    if (this->GetNumberOfThreads() <= 0)
    {
        this->SetNumberOfThreads(tbb::task_scheduler_init::default_num_threads());
    }

    // Set up the TBB task_scheduler
    tbb::task_scheduler_init tbb_init(this->GetNumberOfThreads());

    // Generate the number of Jobs
    // based on the OutputImageDimension, NumberOfThreads and NbReduceDimensions
    this->GenerateNumberOfJobs();

    // Debug Output
    itkDebugMacro(<< "TBB: " << this->GetNumberOfJobs() << "jobs, "
                  << this->GetNumberOfThreads() << "threads;" << std::endl)

            // Do the task decomposition using parallel_for
            tbb::parallel_for(
                tbb::blocked_range<int>(0, this->GetNumberOfJobs()),
                TBBFunctor<TInputImage, TOutputImage>(this, outputSize),
                tbb::simple_partitioner()); // force size 1 step
#else
    // Generate the number of Jobs
    // based on the OutputImageDimension, NumberOfThreads and NbReduceDimensions
    this->GenerateNumberOfJobs();

    // Set up the multithreaded processing
    this->GetMultiThreader()->SetNumberOfThreads( this->GetNumberOfThreads() );
    this->GetMultiThreader()->SetSingleMethod(this->MyThreaderCallback, (void *)this );

    // multithread the execution
    this->GetMultiThreader()->SingleMethodExecute();
#endif // ITK_USE_TBB

    // Call a method that can be overridden by a subclass to perform
    // some calculations after all the threads have completed
    this->AfterThreadedGenerateData();
}

template< typename TInputImage, typename TOutputImage >
const ThreadIdType & TBBImageToImageFilter< TInputImage, TOutputImage >::GetNumberOfThreads() const
{
#ifdef ITK_USE_TBB
    return m_TBBNumberOfThreads;
#else
    return Superclass::GetNumberOfThreads();
#endif // ITK_USE_TBB

}

template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::SetNumberOfThreads(ThreadIdType nbThreads)
{
#ifdef ITK_USE_TBB
    this->m_TBBNumberOfThreads = nbThreads;
#else
    Superclass::SetNumberOfThreads(nbThreads);
#endif // ITK_USE_TBB
}


/**
 * \fn    template< typename TInputImage, typename TOutputImage > unsigned int TBBImageToImageFilter< TInputImage, TOutputImage >::GetNumberOfJobs() const
 *
 * \brief    Gets the number of jobs.
 *
 *             \warning This function only returns a valid value when called from AllocateOutputs,
 *             BeforeThreadedGenerateData(), TBBGenerateData() and AfterThreadedGenerateData(),
 *             ie when the input image is known
 *
 * \author  Amir Jaberzadeh, Benoit Scherrer and Etienne St-Onge
 *
 * \tparam    TInputImage     Type of the input image.
 * \tparam    TOutputImage    Type of the output image.
 *
 * \return    The number of jobs
 **/
template< typename TInputImage, typename TOutputImage >
unsigned int TBBImageToImageFilter< TInputImage, TOutputImage >::GetNumberOfJobs() const
{
    return m_TBBNumberOfJobs;
}


/**
 * \fn  template< typename TInputImage, typename TOutputImage > void TBBImageToImageFilter< TInputImage, TOutputImage >::SetNumberOfJobs(unsigned int nbJobs)
 *
 * \brief   Sets the number of jobs (Internal).
 *
 * \author  Amir Jaberzadeh, Benoit Scherrer and Etienne St-Onge
 *
 * \tparam  TInputImage     Type of the input image.
 * \tparam  TOutputImage    Type of the output image.
 *
 * \return  The number of jobs
 **/
template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::SetNumberOfJobs(unsigned int nbJobs)
{
    this->m_TBBNumberOfJobs = nbJobs;
}



/**
 * \fn  template< typename TInputImage, typename TOutputImage > unsigned int TBBImageToImageFilter< TInputImage, TOutputImage >::GetNbReduceDimensions() const
 *
 * \brief   Gets the number of dimension to separate for the Jobs multithreading
 *
 * \author  Etienne St-Onge
 *
 * \tparam  TInputImage     Type of the input image.
 * \tparam  TOutputImage    Type of the output image.
 *
 * \return  The number of jobs
 **/
template< typename TInputImage, typename TOutputImage >
unsigned int TBBImageToImageFilter< TInputImage, TOutputImage >::GetNbReduceDimensions() const
{
    return m_TBBNbReduceDimensions;
}


/**
 * \fn  template< typename TInputImage, typename TOutputImage > void TBBImageToImageFilter< TInputImage, TOutputImage >::SetNbReduceDimensions(int nbReduceDim)
 *
 * \brief   Set the number of dimension to separate and multithread each section.
 *          (nbReduceDim < 0  : negative number for automatic splitting)
 *
 *          \example : for a 3D image (volume) with the shape 30x10x5
 *          nbReduceDim == 0  : Will generate a single (1) Job with the whole image (size 30x10x5)
 *          nbReduceDim == 1  : Will generate 5 Jobs with the slices (size 30x10)
 *          nbReduceDim == 2  : Will generate 50 Jobs with the lines (size 30)
 *          nbReduceDim == 3  : Will generate 1500 Jobs with each voxel (size 1)
 *
 * \author  Etienne St-Onge
 *
 * \tparam  TInputImage     Type of the input image.
 * \tparam  TOutputImage    Type of the output image.
 *
 * \return  The number of jobs
 **/
template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::SetNbReduceDimensions(int nbReduceDim)
{
    if (nbReduceDim > (int)OutputImageDimension)
    {
        this->m_TBBNbReduceDimensions = (int)OutputImageDimension;
    }
    else
    {
        this->m_TBBNbReduceDimensions = nbReduceDim;
    }
}


/**
 * \fn  template< typename TInputImage, typename TOutputImage > void TBBImageToImageFilter< TInputImage, TOutputImage >::GenerateNumberOfJobs()
 *
 * \brief   Generate the number Jobs based on the NbReduceDimensions
 *              or based on the NumberOfThreads and the Image Dimension (if NbReduceDimensions was not set).
 *
 *          \warning  This function must be called after the NumberOfThreads is set.
 *
 * \author  Etienne St-Onge
 *
 * \tparam  TInputImage     Type of the input image.
 * \tparam  TOutputImage    Type of the output image.
 *
 * \return  The number of jobs
 **/
template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::GenerateNumberOfJobs()
{
    // Get the size of the requested region
    typename TOutputImage::ConstPointer output = static_cast<TOutputImage *>(this->ProcessObject::GetOutput(0));
    typename TOutputImage::SizeType outputSize = output->GetRequestedRegion().GetSize();

    // Generate the number of job
    if (m_TBBNbReduceDimensions < 0)
    {
        // assert (GetNumberOfThreads()>0)
        // This function must be called after the NumberOfThreads is Set

        // Heuristic NbReduceDimensions
        m_TBBNbReduceDimensions = 0;
        m_TBBNumberOfJobs = 1;
        int current_dim = OutputImageDimension-1;

        // Minimum Number of Jobs, based on the Number of thread
        unsigned int minNbJobs = JOB_PER_THREAD_RATIO * this->GetNumberOfThreads();
        while( current_dim >= 0 && m_TBBNumberOfJobs < minNbJobs )
        {
            ++m_TBBNbReduceDimensions;
            m_TBBNumberOfJobs *= outputSize[current_dim];
            --current_dim;
        }
    }
    else
    {
        // If manually chosen m_TBBNbReduceDimensions
        // Fixed (preset NbReduceDimensions)
        m_TBBNumberOfJobs = 1;
        for (unsigned int i = OutputImageDimension - GetNbReduceDimensions(); i < OutputImageDimension; ++i)
        {
            m_TBBNumberOfJobs *= outputSize[i];
        }
    }

}

/**
 * \fn  template< typename TInputImage, typename TOutputImage > void TBBImageToImageFilter< TInputImage, TOutputImage >::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId)
 *
 * \brief   Use *TBBGenerateData()* instead of ThreadedGenerateData with TBBImageToImageFilter
 *
 *          \warning TBBImageToImageFilter doesn't support threadId
 *
 * \author  Etienne St-Onge
 *
 * \tparam  TInputImage     Type of the input image.
 * \tparam  TOutputImage    Type of the output image.
 *
 * \return  The number of jobs
 **/
template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId)
{
    (void)outputRegionForThread;
    (void)threadId;
    itkExceptionMacro(<< this->GetNameOfClass() << "(" << this << "): " << std::endl
                      << "Use 'TBBGenerateData()' instead of 'ThreadedGenerateData()' with TBBImageToImageFilter" << std::endl
                      << "TBBImageToImageFilter doesn't  support ThreadId ");
}

#ifndef ITK_USE_TBB

/**
 * \fn	template< typename TInputImage, typename TOutputImage > ITK_THREAD_RETURN_TYPE MyITKImageToImageFilter< TInputImage, TOutputImage >::MyThreaderCallback( void *arg )
 *
 * \brief	Internal function. Callback method for the multithreader.
 *
 * \author	Benoit Scherrer
 * \date	October 2016
 *
 * \exception	Thrown an exception if an error occurs.
 *
 * \tparam	TInputImage 	Type of the input image.
 * \tparam	TOutputImage	Type of the output image.
 * \param [in,out]	Poiner to the itk::MultiThreader::ThreadInfoStruct
 **/

template< typename TInputImage, typename TOutputImage >
ITK_THREAD_RETURN_TYPE TBBImageToImageFilter< TInputImage, TOutputImage >::MyThreaderCallback( void *arg )
{
    // Get the parameters
    typedef itk::MultiThreader::ThreadInfoStruct  ThreadInfoType;
    ThreadInfoType * infoStruct = static_cast< ThreadInfoType * >( arg );
    Self *instance = (Self *)(infoStruct->UserData);
    const unsigned int threadId = infoStruct->ThreadID;

    // Work on the workpile
    int jobId;
    try {
        while ( (jobId=instance->GetNextJob())>=0 )
        {
            instance->ExecuteJob(jobId);
        }
    }
    catch (itk::ExceptionObject& e)
    {
        std::cout<< "THREAD ID"<<threadId<<" / JOB ID << " << jobId << ": ITK EXCEPTION ERROR CAUGHT"<<std::endl
                 << e.GetDescription() << std::endl << "Cannot continue." << std::endl;
        throw e;
    }
    catch ( ... )
    {
        std::cout<<"THREAD ID"<<threadId<<" / JOB ID << " << jobId << " : UNKNOWN EXCEPTION ERROR." << std::endl
                << "Cannot continue."<< std::endl;
        throw;
    }

    // Exit!
    return ITK_THREAD_RETURN_VALUE;
}

/**
 * \fn	template< typename TInputImage, typename TOutputImage > int MyITKImageToImageFilter< TInputImage, TOutputImage >::GetNextJob()
 *
 * \brief	Gets the next job.
 *
 * \author	Benoit Scherrer
 * \date	October 2016
 *
 * \tparam	TInputImage 	Type of the input image.
 * \tparam	TOutputImage	Type of the output image.
 *
 * \return	The next job&lt;typename t input image,typename t output image &gt;
 **/
template< typename TInputImage, typename TOutputImage >
int TBBImageToImageFilter< TInputImage, TOutputImage >::GetNextJob()
{
    int jobId = -1;

    this->m_JobQueueMutex.Lock();
    if (m_CurrentJobQueueIndex == static_cast<int>(m_TBBNumberOfJobs))
        jobId=-1;
    else
    {
        jobId = m_CurrentJobQueueIndex;
        m_CurrentJobQueueIndex++;
    }
    this->m_JobQueueMutex.Unlock();
    return jobId;
}

/**
 * \fn	template< typename TInputImage, typename TOutputImage > int MyITKImageToImageFilter< TInputImage, TOutputImage >::ExecuteJob( int jobId )
 *
 * \brief	Executes the job identified by jobId.
 *
 * \author	Benoit Scherrer
 * \date	October 2016
 *
 * \tparam	TInputImage 	Type of the input image.
 * \tparam	TOutputImage	Type of the output image.
 * \param	jobId	Identifier for the job.
 *
 * \return	.
 **/
template< typename TInputImage, typename TOutputImage >
void TBBImageToImageFilter< TInputImage, TOutputImage >::ExecuteJob( int jobId )
{
    // Get the size of the requested region
    typename TOutputImage::ConstPointer output = static_cast<TOutputImage *>(this->ProcessObject::GetOutput(0));
    typename TOutputImage::SizeType outputSize = output->GetRequestedRegion().GetSize();
    typename TOutputImage::SizeType size = outputSize;

    typename TOutputImage::IndexType index;
    index.Fill(0);

    if (this->GetNbReduceDimensions() > 0)
    {
        unsigned int i = OutputImageDimension - (unsigned int)this->GetNbReduceDimensions();

        index[i] = jobId;
        size[i] = 1;
        while (i < OutputImageDimension - 1)
        {
            index[i+1] = index[i] / outputSize[i];
            index[i] = index[i] % outputSize[i];
            size[i+1] = 1;
            i++;
        }
    }

    // Construct an itk::ImageRegion
    OutputImageRegionType myRegion(index, size);

    // Run the ThreadedGenerateData method!
    this->ThreadedGenerateData(myRegion, jobId);
}


#endif // ITK_USE_TBB


}  //namespace itk

#endif
