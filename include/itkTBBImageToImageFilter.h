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

namespace itk {

#ifdef ITK_USE_TBB
template< typename TInputImage, typename TOutputImage >
class  TBBFunctor;
#endif // ITK_USE_TBB

/**
  * \class   TBBImageToImageFilter
  *
  * \brief   ImageToImageFilter using Intel Threading Building Blocks (TBB) parallelization
  *          Multithreading with Thread and Job pool
  *          Insight Journal artical: http://www.insight-journal.org/browse/publication/974
  *
  * \details If an imaging filter can be implemented as a TBB multithreaded algorithm,
  *          the filter will provide an implementation of TBBGenerateData().
  *          This superclass will automatically split the output image into a number of pieces,
  *          spawn multiple threads, and call TBBGenerateData() in each thread.
  *          Prior to spawning threads, the BeforeThreadedGenerateData() method is called.
  *          After all the threads have completed, the AfterThreadedGenerateData() method is called.
  *
  * \author  Amir Jaberzadeh, Benoit Scherrer, Etienne St-Onge and Simon Warfield
  *
  * \tparam  TInputImage     Type of the input image.
  * \tparam  TOutputImage    Type of the output image.
  *
  * \sa      ImageToImageFilter
  *
  * \ingroup ITKTBBImageToImageFilter
  **/
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


  // ImageDimension constants
  itkStaticConstMacro(InputImageDimension, unsigned int, TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

public:

  /**
   * \brief Gets the number of dimension to separate for the Jobs multithreading
   *
   */
  unsigned int    GetNbReduceDimensions() const;

  /**
  * \brief   Set the number of dimension to separate and multithread each section.
  *          (nbReduceDim < 0  : negative number for automatic splitting)
  *          \example : for a 3D image (volume) with the shape 30x10x5
  *          nbReduceDim == 0  : Will generate a single (1) Job with the whole image (size 30x10x5)
  *          nbReduceDim == 1  : Will generate 5 Jobs with the slices (size 30x10)
  *          nbReduceDim == 2  : Will generate 50 Jobs with the lines (size 30)
  *          nbReduceDim == 3  : Will generate 1500 Jobs with each voxel (size 1)
  **/
  void            SetNbReduceDimensions(int);

  virtual const ThreadIdType & GetNumberOfThreads() const;
  virtual void SetNumberOfThreads(ThreadIdType);


protected:
  TBBImageToImageFilter();
  ~TBBImageToImageFilter();

  /**
  * \brief   If an imaging filter can be implemented as a TBB multithreaded algorithm,
  *          the filter will provide an implementation of TBBGenerateData().
  *          This superclass will automatically split the output image into a number of pieces,
  *          spawn multiple threads, and call TBBGenerateData() in each thread.
  *          Prior to spawning threads, the BeforeThreadedGenerateData() method is called.
  *          After all the threads have completed, the AfterThreadedGenerateData() method is called.
  *
  *          \warning TBBImageToImageFilter doesn't support threadId
  **/
  virtual void TBBGenerateData(const OutputImageRegionType& outputRegionForThread) = 0;

  /**
 * \brief   Use *TBBGenerateData()* instead of ThreadedGenerateData with TBBImageToImageFilter
 *
 *          \warning TBBImageToImageFilter doesn't support threadId
 **/
  virtual void ThreadedGenerateData(const OutputImageRegionType& outputRegionForThread,
                                    ThreadIdType threadId) ITK_FINAL;

  /**
  * \brief    New default implementation for GenerateData() to use TBB
  **/
  void GenerateData();


  /**
  * \brief    Gets the number of jobs.
  **/
  unsigned int    GetNumberOfJobs() const;
  /**
  * \brief   Sets the number of jobs (Internal).
  **/
  void            SetNumberOfJobs(unsigned int);
  /**
  * \brief   Generate the number Jobs based on the NbReduceDimensions
  *          or based on the NumberOfThreads and the Image Dimension
  *          (if NbReduceDimensions was not set).
  *          \warning  This function must be called after the NumberOfThreads is set.
  **/
  void            GenerateNumberOfJobs();

#ifndef ITK_USE_TBB
  int             GetNextJob();
  void            ExecuteJob( int jobId );
  void            ResetJobQueue();
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
