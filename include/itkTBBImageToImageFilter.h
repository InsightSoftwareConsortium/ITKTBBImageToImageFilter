#ifndef itkTBBImageToImageFilter_h
#define itkTBBImageToImageFilter_h

#include <itkImageToImageFilter.h>


/************************************************************************************************
* \class	TBBImageToImageFilter
*
* \brief	TBBImageToImageFilter.
*
* \author   Amir Jaberzadeh, Benoit Scherrer and Etienne St-Onge
* \date     September 2016
*************************************************************************************************/

namespace itk {

#ifdef ITK_USE_TBB
template< typename TInputImage, typename TOutputImage >
class  TBBFunctor;
#endif // ITK_USE_TBB

/**
 * \class	TBBImageToImageFilter
 *
 * \brief   ImageToImageFilter using Intel Threading Building Blocks (TBB) parallelization
 *          Multithreading with Thread and Job pool
 *
 * \author  Amir Jaberzadeh, Benoit Scherrer and Etienne St-Onge
 *
 * \tparam	TInputImage 	Type of the input image.
 * \tparam	TOutputImage	Type of the output image.
 **/
template< typename TInputImage, typename TOutputImage >
class TBBImageToImageFilter : public ImageToImageFilter< TInputImage, TOutputImage>
{

#ifdef ITK_USE_TBB
    friend class TBBFunctor<TInputImage,TOutputImage>;
#endif // ITK_USE_TBB

public:
    // Standard class typedefs.
    typedef TBBImageToImageFilter							Self;
    typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
    typedef SmartPointer< Self >							Pointer;
    typedef SmartPointer< const Self >						ConstPointer;

    // Run-time type information (and related methods).
    itkTypeMacro(TBBImageToImageFilter, ImageToImageFilter);

    // Superclass typedefs.
    typedef typename Superclass::OutputImageRegionType      OutputImageRegionType;
    typedef typename Superclass::OutputImagePixelType       OutputImagePixelType;

    // Some convenient typedefs. The same as itk::ImageToImageFilter
    typedef TInputImage                                     InputImageType;
    typedef typename InputImageType::Pointer                InputImagePointer;
    typedef typename InputImageType::ConstPointer           InputImageConstPointer;
    typedef typename InputImageType::RegionType             InputImageRegionType;
    typedef typename InputImageType::PixelType              InputImagePixelType;
    typedef typename TInputImage::SizeType                  InputImageSizeType;

    typedef TOutputImage                                    OutputImageType;
    typedef typename TOutputImage::SizeType                 OutputImageSizeType;


    // ImageDimension constants
    itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

public:
    /**
     * \fn  template< typename TInputImage, typename TOutputImage >
     *      void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread, ThreadIdType threadId)
     *
     * \brief   If an imaging filter can be implemented as a TBB multithreaded algorithm,
     *          the filter will provide an implementation of TBBGenerateData().
     *          This superclass will automatically split the output image into a number of pieces,
     *          spawn multiple threads, and call TBBGenerateData() in each thread.
     *          Prior to spawning threads, the BeforeThreadedGenerateData() method is called.
     *          After all the threads have completed, the AfterThreadedGenerateData() method is called.
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

    virtual void TBBGenerateData(const OutputImageRegionType& outputRegionForThread) = 0;
    // use TBBGenerateData instead of ThreadedGenerateData
    virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                                      ThreadIdType threadId) ITK_FINAL;

    void GenerateData();

    unsigned int    GetNbReduceDimensions() const;
    void            SetNbReduceDimensions(int);

    virtual const ThreadIdType & GetNumberOfThreads() const;
    virtual void SetNumberOfThreads(ThreadIdType);


protected:
    TBBImageToImageFilter();
    ~TBBImageToImageFilter();

    unsigned int    GetNumberOfJobs() const;
    void            SetNumberOfJobs(unsigned int);
    void            GenerateNumberOfJobs();

#ifndef ITK_USE_TBB
    int				GetNextJob();
    void			ExecuteJob( int jobId );
    void			ResetJobQueue();
    static ITK_THREAD_RETURN_TYPE MyThreaderCallback( void *arg );
#endif // ITK_USE_TBB

private:
    TBBImageToImageFilter(const Self &); //purposely not implemented
    void operator=(const Self &); //purposely not implemented

#ifndef ITK_USE_TBB
    int                         m_CurrentJobQueueIndex;
    itk::SimpleFastMutexLock    m_JobQueueMutex;
#endif // ITK_USE_TBB
    unsigned int                m_TBBNumberOfJobs;
    unsigned int                m_TBBNumberOfThreads;
    int                         m_TBBNbReduceDimensions;
};

}   //end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTBBImageToImageFilter.hxx"
#endif // ITK_MANUAL_INSTANTIATION

#endif // itkTBBImageToImageFilter_h
