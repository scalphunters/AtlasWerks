/* ================================================================
 *
 * AtlasWerks Project
 *
 * Copyright (c) Sarang C. Joshi, Bradley C. Davis, J. Samuel Preston,
 * Linh K. Ha. All rights reserved.  See Copyright.txt or for details.
 *
 * This software is distributed WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the above copyright notice for more information.
 *
 * ================================================================ */


#ifndef __GREEDY_ATLAS_BUILDER_H__
#define __GREEDY_ATLAS_BUILDER_H__

#include "AtlasBuilderInterface.h"
#include "GreedyAtlasParam.h"
#include "Energy.h"
#include "EnergyHistory.h"
#include "ImagePCA.h"
#include "barriers.h"

#ifdef MPI_ENABLED
#include <mpi.h>
#endif // MPI_ENABLED

template<class AtlasManagerType>
class GreedyAtlasBuilder
  : public AtlasBuilderInterface
{

  typedef void* (*PTHREAD_STARTFUNC)(void*);
  
public:
  
  typedef typename AtlasManagerType::MultiscaleManagerType MultiscaleManagerType;
  typedef typename AtlasManagerType::DeformationDataType DeformationDataType;
  typedef typename AtlasManagerType::AtlasThreadType AtlasThreadType;

  // Constructor / Destructor
  GreedyAtlasBuilder(const WeightedImageSet &imageSet,
		   GreedyAtlasParam &param,
		   unsigned int nThreads,
		   unsigned int nodeId = 0,
		   unsigned int nNodes = 1,
		   unsigned int nTotalImages = 0);
  ~GreedyAtlasBuilder();

  virtual void BuildAtlas();

  void GenerateOutput();

  const MultiscaleManagerType &GetScaleManager() const { return *mScaleManager; }

  // called from individual threads, performs thread synchronization
  void ComputeMean();
  void SetScaleLevel(int scaleLevel);
  
  DeformationDataType &GetDeformationData(unsigned int idx);
  
  /** Called at the beginning of a scale level */
  virtual void BeginScaleLevel(int scaleLevel);
  
  /** Called at the end of an iteration (after new mean image is generated) */
  virtual void FinishIteration(int iteration);
  
  /** Called at the end of a scale level */
  virtual void FinishScaleLevel(int scaleLevel);

  /** Given the local (node) deformation index, return the global deformation index */
  int LocalToGlobalMapping(int localIdx);

  void WriteMeanImage(std::string title="");

  void WriteDefImages(std::string title="");
    
  void WriteDefMean(std::string title="");
    
  void WriteDefFieldsMeanToIm(std::string title="");
    
  void WriteDefFieldsImToMean(std::string title="");

  void WriteEnergy(std::string title="");

  void WriteIndividualEnergies(std::string title="");

  void WritePCAData(std::string title="");

  void WritePCAProjections(std::string title="");

protected:

  void SumAcrossNodes();

  void ComputePCA();

  std::vector<AtlasThreadType*> mAtlasThread;
  
  GreedyAtlasParam &mParam;

  unsigned int mNScaleLevels;
  
  MultiscaleManagerType *mScaleManager;
  
  RealImage *mMeanImage;

  // only initialized / used if built with MPI support
  RealImage *mMPIBuff;
  
  EnergyHistory mEnergyHistory;
  Energy mCurIterEnergy;

  pthread_barrier_t mBarrier;

  // actual per-deformation data
  std::vector<DeformationDataType*> mDeformationData;

  // data for pca
  std::vector<RealImage*> mIndividualMeans;
  ImagePCA *mImagePCA;
  
};

#ifndef SWIG
#include "GreedyAtlasBuilder.txx"
#endif // !SWIG

#endif // __GREEDY_ATLAS_BUILDER_H__
