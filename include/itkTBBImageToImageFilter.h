/*=========================================================================
*
*  Copyright Insight Software Consortium
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*         http://www.apache.org/licenses/LICENSE-2.0.txt
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*=========================================================================*/

#ifndef itkTBBImageToImageFilter_h
#define itkTBBImageToImageFilter_h

#include <itkImageToImageFilter.h>


namespace itk
{
#ifdef ITK_USE_TBB
template< typename TInputImage, typename TOutputImage >
class TBBFunctor;
#endif // ITK_USE_TBB

/**
 * \class TBBImageToImageFilter
 *
 * \brief ImageToImageFilter using Intel Threading Building Blocks (TBB) parallelization
 *        Multithreading with Thread and Job pool
 *
 * Insight Journal article: http://www.insight-journal.org/browse/publication/974
 *
 * If an imaging filter can be implemented as a TBB multithreaded algorithm,
 * the filter will provide an implementation of TBBGenerateData().
 * This superclass will automatically split the output image into a number of pieces,
 * spawn multiple threads, and call TBBGenerateData() in each thread.
 * Prior to spawning threads, the BeforeThreadedGenerateData() method is called.
 * After all the threads have completed, the AfterThreadedGenerateData() method is called.
 *
 * \sa ImageToImageFilter
 *
 * \ingroup TBBImageToImageFilter
 *
 * \tparam TInputImage     Type of the input image.
 * \tparam TOutputImage    Type of the output image.
 *
 * \author Amir Jaberzadeh, Benoit Scherrer, Etienne St-Onge and Simon Warfield
 */
template< typename TInputImage, typename TOutputImage >
class TBBImageToImageFilter : public ImageToImageFilter< TInputImage, TOutputImage>
{

#ifdef ITK_USE_TBB
  friend class TBBFunctor<TInputImage,TOutputImage>;
#endif // ITK_USE_TBB

public:
  // Standard class typedefs.
  typedef TBBImageToImageFilter                           Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >                            Pointer;
  typedef SmartPointer< const Self >                      ConstPointer;

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

  /** Type to count and reference number of jobs */
  typedef unsigned int        JobIdType;
  /** Type to count the number of dimesions to separate the Jobs multithreading */
  typedef int                 DimensionReductionType;


  // ImageDimension constants
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);
  itkStaticConstMacro(JobPerThreadRatio, unsigned int, 20);

public:

  /** Gets the number of dimension to separate for the Jobs multithreading */
  itkGetConstMacro(NumberOfDimensionToReduce, DimensionReductionType);

  /** Set the number of dimension to separate and multithread each section.
   * (nbReduceDim < 0  : negative number for automatic splitting)
   * \example : for a 3D image (volume) with the shape 30x10x5
   * nbReduceDim == 0  : Will generate a single (1) Job with the whole image (size 30x10x5)
   * nbReduceDim == 1  : Will generate 5 Jobs with the slices (size 30x10)
   * nbReduceDim == 2  : Will generate 50 Jobs with the lines (size 30)
   * nbReduceDim == 3  : Will generate 1500 Jobs with each voxel (size 1) */
  void SetNumberOfDimensionToReduce(DimensionReductionType NumberOfDimensionToReduce);

  // redefinition so we can use our own member if ITK_USE_TBB is defined
  const ThreadIdType & GetNumberOfThreads() const override;
  void SetNumberOfThreads(ThreadIdType) override;


protected:
  TBBImageToImageFilter();
  ~TBBImageToImageFilter() override;

  /** If an imaging filter can be implemented as a TBB multithreaded algorithm,
   * the filter will provide an implementation of TBBGenerateData().
   * This superclass will automatically split the output image into a number of pieces,
   * spawn multiple threads, and call TBBGenerateData() in each thread.
   * Prior to spawning threads, the BeforeThreadedGenerateData() method is called.
   * After all the threads have completed, the AfterThreadedGenerateData() method is called.
   * \warning TBBImageToImageFilter doesn't support threadId
   **/
  virtual void TBBGenerateData(const OutputImageRegionType& outputRegionForThread) = 0;

  /** Use *TBBGenerateData()* instead of ThreadedGenerateData with TBBImageToImageFilter
  * \warning TBBImageToImageFilter doesn't support threadId */
  void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                                    ThreadIdType threadId) ITK_FINAL;

  /** New default implementation for GenerateData() to use TBB */
  void GenerateData() override;


  /** Set/Get the number of jobs (Internal). */
  itkGetConstMacro( NumberOfJobs, JobIdType );
  itkSetMacro( NumberOfJobs, JobIdType );

  /** Generate the number Jobs based on the NbReduceDimensions
   * or based on the NumberOfThreads and the Image Dimension
   * (if NbReduceDimensions was not set).
   * \warning  This function must be called after the NumberOfThreads is set. */
  void GenerateNumberOfJobs();

#ifndef ITK_USE_TBB
  /** Gets the next job.*/
  int GetNextJob();

  /** Executes the job identified by jobId. */
  void ExecuteJob( int jobId );

  /** Reset the job queu index to 0.*/
  void ResetJobQueue();

  /** Internal function. Callback method for the multithreader.
   *
   * \exception Thrown an exception if an error occurs.
   * \param     [in,out]   Poiner to the itk::MultiThreader::ThreadInfoStruct */
  static ITK_THREAD_RETURN_TYPE MyThreaderCallback( void *arg );
#endif // ITK_USE_TBB

  void PrintSelf(std::ostream &os, Indent indent) const override;

private:
  ITK_DISALLOW_COPY_AND_ASSIGN(TBBImageToImageFilter);

  JobIdType                   m_NumberOfJobs;
  DimensionReductionType      m_NumberOfDimensionToReduce;

#ifndef ITK_USE_TBB
  int                         m_CurrentJobQueueIndex;
  itk::SimpleFastMutexLock    m_JobQueueMutex;
#else
  // Use to ensure that the number of thread can't be modify by one of the classs inherited.
  ThreadIdType                m_TBBNumberOfThreads;
#endif // ITK_USE_TBB

};
}   //end namespace itk

#ifdef ITK_USE_TBB
#include <tbb/blocked_range.h>

namespace itk {

/**
 * \class TBBFunctor
 *
 * \brief  TBB functor to execute jobs in parallel.
 *
 * \sa TBBImageToImageFilter
 *
 * \ingroup TBBImageToImageFilter
 *
 * \tparam TInputImage     Type of the input image.
 * \tparam TOutputImage    Type of the output image.
 *
 * \author Amir Jaberzadeh, Benoit Scherrer, Etienne St-Onge and Simon Warfield
 *
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

  typedef itk::TBBImageToImageFilter<TInputImage,TOutputImage> TbbImageFilterType;

  TBBFunctor(TbbImageFilterType *tbbFilter, const OutputImageSizeType& outputSize):
    m_TBBFilter(tbbFilter), m_OutputSize(outputSize)
  {
  }

  void operator() ( const tbb::blocked_range<int>& r ) const;

private:
  TbbImageFilterType *m_TBBFilter;
  OutputImageSizeType m_OutputSize;
};

} // itk
#endif // ITK_USE_TBB

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkTBBImageToImageFilter.hxx"
#endif // ITK_MANUAL_INSTANTIATION

#endif // itkTBBImageToImageFilter_h
