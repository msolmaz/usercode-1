// -*- C++ -*-
//
// Package:    TrackingEfficiencyFromCosmics
// Class:      TrackingEfficiencyFromCosmics
// 
/**\class TrackingEfficiencyFromCosmics TrackingEfficiencyFromCosmics.cc Analysis/TrackingEfficiencyFromCosmics/src/TrackingEfficiencyFromCosmics.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Marco De Mattia,40 3-B32,+41227671551,
//         Created:  Wed May 25 16:44:02 CEST 2011
// $Id: TrackingEfficiencyFromCosmics.cc,v 1.12 2011/07/05 14:43:59 demattia Exp $
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Math/interface/deltaR.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "SimDataFormats/Track/interface/SimTrackContainer.h"

// #include "DataFormats/BTauReco/interface/SoftLeptonTagInfo.h"
// #include "DataFormats/BTauReco/interface/SecondaryVertexTagInfo.h"
// #include "DataFormats/CLHEP/interface/AlgebraicObjects.h"
#include "DataFormats/GeometryCommonDetAlgo/interface/Measurement1D.h"
#include "DataFormats/Math/interface/Error.h"
// #include "DataFormats/TrackReco/interface/DeDxData.h"
#include "DataFormats/TrackReco/interface/TrackBase.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"
// #include "DataFormats/PatCandidates/interface/TriggerEvent.h"
// #include "PhysicsTools/PatUtils/interface/TriggerHelper.h"
// #include "SimTracker/Records/interface/TrackAssociatorRecord.h"
// #include "SimTracker/TrackAssociation/interface/TrackAssociatorBase.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalImpactPointExtrapolator.h"
#include "TrackingTools/IPTools/interface/IPTools.h"
#include "TrackingTools/PatternTools/interface/TransverseImpactPointExtrapolator.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
#include <TVector3.h>

#include "Analysis/TrackingEfficiencyFromCosmics/interface/AssociatorByDeltaR.h"
#include "Analysis/TrackingEfficiencyFromCosmics/interface/ControlPlots.h"
#include "Analysis/TrackingEfficiencyFromCosmics/interface/Efficiency.h"
#include "Analysis/TrackingEfficiencyFromCosmics/interface/EfficiencyTree.h"

#include <boost/foreach.hpp>

//
// class declaration
//

class TrackingEfficiencyFromCosmics : public edm::EDAnalyzer {
public:
  explicit TrackingEfficiencyFromCosmics(const edm::ParameterSet&);
  ~TrackingEfficiencyFromCosmics();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob();
  virtual void analyze(const edm::Event&, const edm::EventSetup&);
  virtual void endJob();

  virtual void beginRun(edm::Run const&, edm::EventSetup const&);
  virtual void endRun(edm::Run const&, edm::EventSetup const&);
  virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
  virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

  const reco::GenParticle * takeStableMuon(const reco::GenParticleCollection & genParticles);
  void impactParameterForGen(const reco::GenParticle & genMuon, const math::XYZPoint & genVertex,
                             const int genCharge, const MagneticField * mf);


  void dumpGenParticleInfo(const reco::GenParticle & genParticle);
  void dumpTrackInfo(const reco::Track & track, const unsigned int trackNumber);

  // ----------member data ---------------------------
  // double maxDeltaR_;
  TH1F * hMinDeltaR_, * hSimMinDeltaR_;
  TH1F * hMinTrackToGenDeltaR_, * hMinStaMuonToGenDeltaR_;
  TH1F * hStandAloneToGenDeltaDxy_, * hStandAloneToGenDeltaDz_, * hTrackToGenDeltaDxy_, * hTrackToGenDeltaDz_;
  reco::Track::TrackQuality quality_;
  bool useMCtruth_;

  std::auto_ptr<AssociatorByDeltaR> associatorByDeltaR_;
  std::auto_ptr<AssociatorByDeltaR> simAssociatorByDeltaR_;
  std::auto_ptr<ControlPlots> controlPlotsGeneralTracks_;
  std::auto_ptr<ControlPlots> controlPlotsStandAloneMuons_;
  std::auto_ptr<Efficiency> genToStandAloneEfficiency_;
  std::auto_ptr<Efficiency> genToTrackEfficiency_;
  std::auto_ptr<Efficiency> efficiency_;
  boost::shared_array<double> variables_;
  unsigned int nBins_;
  std::string effOutputFileName_;
  std::string genToStandAloneEffOutputFileName_;
  std::string genToTrackEffOutputFileName_;
  AnalyticalImpactPointExtrapolator * analyticalExtrapolator_;
  TransverseImpactPointExtrapolator * transverseExtrapolator_;
  AlgebraicSymMatrix55 nullCovariance_;
  std::pair<double, double> dxy_;
  std::pair<double, double> dz_;
  std::pair<double, double> dxyz_;
};

TrackingEfficiencyFromCosmics::TrackingEfficiencyFromCosmics(const edm::ParameterSet& iConfig) :
  useMCtruth_(iConfig.getParameter<bool>("UseMCtruth")),
  effOutputFileName_(iConfig.getParameter<std::string>("EffOutputFileName")),
  genToStandAloneEffOutputFileName_(iConfig.getParameter<std::string>("GenToStandAloneEffOutputFileName")),
  genToTrackEffOutputFileName_(iConfig.getParameter<std::string>("GenToTrackEffOutputFileName"))
{
  associatorByDeltaR_.reset(new AssociatorByDeltaR(iConfig.getParameter<double>("MaxDeltaR")));
  simAssociatorByDeltaR_.reset(new AssociatorByDeltaR(iConfig.getParameter<double>("SimMaxDeltaR"), false));

  nBins_ = 100;

  // Build the object to compute the efficiency
  std::vector<Efficiency::Parameters> pars;
  pars.push_back(Efficiency::Parameters("dxy", nBins_, 0, 100));
  pars.push_back(Efficiency::Parameters("dz", nBins_, 0, 100));
  pars.push_back(Efficiency::Parameters("pt", nBins_, 0, 1000));
  genToStandAloneEfficiency_.reset(new Efficiency(pars));
  genToTrackEfficiency_.reset(new Efficiency(pars));
  efficiency_.reset(new Efficiency(pars));
  variables_.reset(new double[3]);

  // maxDeltaR_ = iConfig.getParameter<double>("MaxDeltaR");

  // Initialize the nullCovariance matrix
  for( unsigned int i=0; i<5; ++i ) {
    for( unsigned int j=0; j<5; ++j ) {
      nullCovariance_(i,j) = 0;
    }
  }
}

TrackingEfficiencyFromCosmics::~TrackingEfficiencyFromCosmics() {}

void TrackingEfficiencyFromCosmics::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::ESHandle<MagneticField> theMF;
  iSetup.get<IdealMagneticFieldRecord>().get(theMF);
  MagneticField * mf = const_cast<MagneticField*>(&*theMF);
  transverseExtrapolator_ = new TransverseImpactPointExtrapolator(mf);
  analyticalExtrapolator_ = new AnalyticalImpactPointExtrapolator(mf);

  edm::Handle<reco::TrackCollection> tracks;
  iEvent.getByLabel("generalTracks", tracks);
  edm::Handle<reco::TrackCollection> staMuons;
  iEvent.getByLabel("standAloneMuons", staMuons);

  // Gen Particles
  edm::Handle<reco::GenParticleCollection> genParticles;
  if( useMCtruth_ ) {
    iEvent.getByLabel("genParticles", genParticles);
    const reco::GenParticle * stableMuon = takeStableMuon(*genParticles);

    // std::map<const math::XYZTLorentzVectorD *, const reco::Track *> simMatchesMap;

    // Compute impact parameters for generator particle
    impactParameterForGen(*stableMuon, stableMuon->vertex(), stableMuon->charge(), mf);

    // // Find the simTracks (for IP)
    // edm::Handle<edm::SimTrackContainer> simTracks;
    // iEvent.getByLabel("g4SimHits", simTracks);

    std::map<const math::XYZTLorentzVectorD *, const reco::Track *> simMatchesMap;
    // simAssociatorByDeltaR_->fillAssociationMap(*simTracks, *tracks, simMatchesMap, hSimMinDeltaR_);

    // Compute efficiency from MC truth
    // std::map<const math::XYZTLorentzVectorD *, const reco::Track *>::const_iterator it = simMatchesMap.begin();
    // for( ; it != simMatchesMap.end(); ++it ) {
//      variables_[0] = it->first->pt();
//      bool found = false;
//      if( it->second != 0 ) found = true;
//      genEfficiency_->fill(variables_, found);
//    }

    // Gen muon values
    variables_[0] = dxy_.first;
    variables_[1] = dz_.first;
    variables_[2] = stableMuon->pt();


    // Compute efficiency for track vs MC-truth
    if( tracks->size() == 0 ) {
      // Do it twice as we expect two standalone muons and we reconstruct none
      genToTrackEfficiency_->fill(variables_, false);
      genToTrackEfficiency_->fill(variables_, false);
    }
    else if( tracks->size() == 1 ) {
      genToTrackEfficiency_->fill(variables_, false);
    }
    if( tracks->size() > 2 ) {
      std::cout << "How did we get three tracks in simulation from a single cosmic track?" << std::endl;
    }
    BOOST_FOREACH( const reco::Track & track, *tracks ) {
      hMinTrackToGenDeltaR_->Fill(reco::deltaR(*stableMuon, track));
      hTrackToGenDeltaDxy_->Fill(track.dxy() - dxy_.first);
      hTrackToGenDeltaDz_->Fill(track.dz() - dz_.first);
      genToTrackEfficiency_->fill(variables_, true);
    }

    // Compute efficiency for standalone vs MC-truth
    if( staMuons->size() == 0 ) {
      // Do it twice as we expect two standalone muons and we reconstruct none
      genToStandAloneEfficiency_->fill(variables_, false);
      genToStandAloneEfficiency_->fill(variables_, false);
    }
    else if( staMuons->size() == 1 ) {
      genToStandAloneEfficiency_->fill(variables_, false);
    }
    if( staMuons->size() > 2 ) {
      std::cout << "How did we get three standAloneMuons in simulation from a single cosmic track?" << std::endl;
    }
    BOOST_FOREACH( const reco::Track & staMuon, *staMuons ) {
      hMinStaMuonToGenDeltaR_->Fill(reco::deltaR(*stableMuon, staMuon));
      hStandAloneToGenDeltaDxy_->Fill(staMuon.dxy() - dxy_.first);
      hStandAloneToGenDeltaDz_->Fill(staMuon.dz() - dz_.first);
      genToStandAloneEfficiency_->fill(variables_, true);
    }
  }

  // Fill all control plots
  controlPlotsGeneralTracks_->fillControlPlots(*tracks);
  controlPlotsStandAloneMuons_->fillControlPlots(*staMuons);

  // Association map of StandAloneMuons and TrackerTracks
  std::map<const reco::Track *, const reco::Track *> matchesMap;

  if( staMuons->size() > 0 ) {
    // associatorByDeltaR_->fillAssociationMap(tracks, staMuons, matchesMap, hMinDeltaR_);
    // associatorByDeltaR_->fillAssociationMap(*tracks, *staMuons, matchesMap, hMinDeltaR_);
    associatorByDeltaR_->fillAssociationMap(*staMuons, *tracks, matchesMap, hMinDeltaR_);

    bool found = false;
    std::map<const reco::Track *, const reco::Track *>::const_iterator it = matchesMap.begin();
    for( ; it != matchesMap.end(); ++it ) {
      found = false;
      if( it->second == 0 ) {
        std::cout << "NO match found for standAlone with pt = " << it->first->pt() << std::endl;
        std::cout << "and dxy = " << fabs(it->first->dxy()) << std::endl;
      }
      else {
        found = true;
        std::cout << "MATCH FOUND for standAlone with pt = " << it->first->pt() << ", matches with track of pt = " << it->second->pt() << std::endl;
        std::cout << "and dxy = " << fabs(it->first->dxy()) << std::endl;
      }
      variables_[0] = fabs(it->first->dxy());
      variables_[1] = fabs(it->first->dz());
      variables_[2] = fabs(it->first->pt());
      efficiency_->fill(variables_, found);
    }
  }

  // Compute efficiency as number of times a track matching the standalone is found vs number of standalone.
  // Do this as a function of several variables.


#ifdef THIS_IS_AN_EVENT_EXAMPLE
  Handle<ExampleData> pIn;
  iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  ESHandle<SetupData> pSetup;
  iSetup.get<SetupRecord>().get(pSetup);
#endif
}

const reco::GenParticle * TrackingEfficiencyFromCosmics::takeStableMuon(const reco::GenParticleCollection & genParticles)
{
  for( reco::GenParticleCollection::const_iterator it = genParticles.begin();
       it != genParticles.end(); ++it ) {
    // dumpGenParticleInfo(*it);
    if( it->status() == 1 ) return( &*it );
  }
  return 0;
}

// ------------ method called once each job just before starting event loop  ------------
void TrackingEfficiencyFromCosmics::beginJob()
{
  edm::Service<TFileService> fileService;
  hMinDeltaR_ = fileService->make<TH1F>("minDeltaR","#Delta R between standalone muon and closest track",500,0,5);
  hSimMinDeltaR_ = fileService->make<TH1F>("simMinDeltaR","#Delta R between simTrack and closest track",500,0,5);
  hMinTrackToGenDeltaR_ = fileService->make<TH1F>("minTrackGenDeltaR","#Delta R between gen muon and closest track",500,0,5);
  hMinStaMuonToGenDeltaR_ = fileService->make<TH1F>("minStaMunoGenDeltaR","#Delta R between gen muon and closest standAloneMuon",500,0,5);

  hStandAloneToGenDeltaDxy_ = fileService->make<TH1F>("standAloneToGenDeltaDxy", "#Delta(dxy) standAlone - gen", 100, -100, 100);
  hStandAloneToGenDeltaDz_ = fileService->make<TH1F>("standAloneToGenDeltaDz", "#Delta(dz) standAlone - gen", 100, -100, 100);
  hTrackToGenDeltaDxy_ = fileService->make<TH1F>("trackToGenDeltaDxy", "#Delta(dxy) track - gen", 100, -100, 100);
  hTrackToGenDeltaDz_ = fileService->make<TH1F>("trackToGenDeltaDz", "#Delta(dz) track - gen", 100, -100, 100);

  controlPlotsGeneralTracks_.reset(new ControlPlots(fileService, "generalTracks"));
  controlPlotsStandAloneMuons_.reset(new ControlPlots(fileService, "standAloneMuons"));
}

// ------------ method called once each job just after ending the event loop  ------------
void TrackingEfficiencyFromCosmics::endJob() 
{
//  if( useMCtruth_ ) {
//    for( unsigned int i=0; i<nBins_; ++i ) {
//      std::cout << "genEfficiency["<<i<<"] (vs pt) = " << genEfficiency_->getEff(i) << " +/- " << genEfficiency_->getEffError(i) << std::endl;
//    }
//  }

//  boost::shared_array<unsigned int> vKeep(new int[2]);
//  vKeep[0] = 0;
//  vKeep[1] = -1;
//  boost::shared_ptr<Efficiency> newEff(efficiency_->project(vKeep));
//  for( unsigned int i=0; i<nBins_; ++i ) {
//    std::cout << "reco eff["<<i<<"] = " << newEff->getEff(i) << " +/- " << newEff->getEffError(i) << std::endl;
//  }

//  for( unsigned int i=0; i<nBins_; ++i ) {
//    std::cout << "reco eff["<<i<<"] = " << efficiency_->getEff(i) << " +/- " << efficiency_->getEffError(i) << std::endl;
//  }


  EfficiencyTree tree;
  tree.writeTree(effOutputFileName_, &*efficiency_);
  // tree.writeTree(effOutputFileName_, &*newEff);

  EfficiencyTree genToStandAloneTree;
  genToStandAloneTree.writeTree(genToStandAloneEffOutputFileName_, &*genToStandAloneEfficiency_);

  EfficiencyTree genToTrackTree;
  genToTrackTree.writeTree(genToTrackEffOutputFileName_, &*genToTrackEfficiency_);
}

// ------------ method called when starting to processes a run  ------------
void TrackingEfficiencyFromCosmics::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void TrackingEfficiencyFromCosmics::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void TrackingEfficiencyFromCosmics::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void TrackingEfficiencyFromCosmics::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void TrackingEfficiencyFromCosmics::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

void TrackingEfficiencyFromCosmics::impactParameterForGen(const reco::GenParticle & genMuon, const math::XYZPoint & genVertex,
                                                          const int genCharge, const MagneticField * mf)
{
  // std::cout << "impactParameterForGen" << std::endl;
  VertexDistanceXY distXY;
  VertexDistance3D dist3D;

  //  std::vector<double> dxy_PV;
  //  std::vector<double> dxyError_PV;
  //  std::vector<double> dz_PV;
  //  std::vector<double> dzError_PV;
  //  std::vector<double> dxyz_PV;
  //  std::vector<double> dxyzError_PV;

  TVector3 genMomentum(0,0,0);
  genMomentum.SetPtEtaPhi(genMuon.pt(),genMuon.eta(),genMuon.phi());
  // std::cout << "genVertex = (" << genVertex.x() << "," << genVertex.y() << "," << genVertex.z() << ")" << std::endl;
  FreeTrajectoryState ftsAtProduction(GlobalPoint(genVertex.x(),genVertex.y(),genVertex.z()),
                                      GlobalVector(genMomentum.x(),genMomentum.y(),genMomentum.z()),
                                      TrackCharge(genCharge), mf);

  TrajectoryStateOnSurface transverseTSOS_ = transverseExtrapolator_->extrapolate(ftsAtProduction, GlobalPoint(0,0,0));
  TrajectoryStateOnSurface analyticalTSOS_ = analyticalExtrapolator_->extrapolate(ftsAtProduction, GlobalPoint(0,0,0));

  if(transverseTSOS_.isValid()) {
    TrajectoryStateOnSurface transverseTSOS(transverseTSOS_.localParameters(), LocalTrajectoryError(nullCovariance_),
                                            transverseTSOS_.surface(), transverseTSOS_.magneticField(), transverseTSOS_.weight());
    // Reco vertex default constructed to (0,0,0)
    std::pair<bool,Measurement1D> dxy = IPTools::absoluteImpactParameter(transverseTSOS, reco::Vertex(), distXY);
    std::cout << "dxy = " << dxy.second.value() << " +/- " << dxy.second.error() << std::endl;
    dxy_.first = dxy.second.value();
    dxy_.second = dxy.second.error();
    // dxy_PV.push_back(dxy.second.value());
    // dxyError_PV.push_back(dxy.second.error());
    // Formulae for dz taken from TrackingTools/IPTools/src/ImpactParameterComputer.cc
    dz_.first = transverseTSOS.globalPosition().z();
    dz_.second = transverseTSOS.cartesianError().position().czz();
    // dz_PV.push_back(transverseTSOS.globalPosition().z());
    // double refZErr = transverseTSOS.cartesianError().position().czz();
    // dzError_PV.push_back(refZErr);
  }
  else {
    // dxy_PV.push_back(65535);
    // dxyError_PV.push_back(65535);
    // dz_PV.push_back(65535);
    // dzError_PV.push_back(65535);
    dxy_.first = 65535;
    dxy_.second = 65535;
    dz_.first = 65535;
    dz_.second = 65535;
  }
  if(analyticalTSOS_.isValid()) {
    // analyticalTSOS_ has no errors defined. Explicitly set the errors to 0 for the genparticle state
    TrajectoryStateOnSurface analyticalTSOS(analyticalTSOS_.localParameters(), LocalTrajectoryError(nullCovariance_), analyticalTSOS_.surface(), analyticalTSOS_.magneticField()
                                            , analyticalTSOS_.weight());
    std::pair<bool,Measurement1D> dxyz = IPTools::absoluteImpactParameter(analyticalTSOS, reco::Vertex(), dist3D);
    dxyz_.first = dxyz.second.value();
    dxyz_.second = dxyz.second.error();
    // dxyz_PV.push_back(dxyz.second.value());
    // dxyzError_PV.push_back(dxyz.second.error());
  }
  else {
    // dxyz_PV.push_back(65535);
    // dxyzError_PV.push_back(65535);
    dxyz_.first = 65535;
    dxyz_.second = 65535;
  }
  // std::cout << "dxy_PV[0] = " << dxy_PV[0] << std::endl;
}

void TrackingEfficiencyFromCosmics::dumpGenParticleInfo(const reco::GenParticle & genParticle)
{
  std::cout << "pdgid = " << genParticle.pdgId() << std::endl;
  std::cout << "status = " << genParticle.status() << std::endl;
  std::cout << "pt = " << genParticle.pt() << ", eta = " << genParticle.eta() << ", phi = " << genParticle.phi() << std::endl;
}

void TrackingEfficiencyFromCosmics::dumpTrackInfo(const reco::Track & track, const unsigned int trackNumber)
{
  track.quality(quality_);
  std::stringstream ss;
  ss << "track["<<trackNumber<<"]" << std::endl <<
        "algoName = " << track.algoName() << std::endl <<
        "pt = " << track.pt() << std::endl <<
        "eta = " << track.eta() << std::endl <<
        "phi = " << track.phi() << std::endl <<
        "number of hits = " << track.recHitsSize() << std::endl <<
        "number of valid hits = " << track.found() << std::endl <<
        "number of invalid hits = " << track.lost() << std::endl <<
        "quality = " << track.qualityName(quality_) << std::endl <<
        "d0 = " << track.d0() << std::endl <<
        "d0/d0Error = " << track.d0Error() << std::endl <<
        "dz = " << track.dz() << std::endl <<
        "dz/dzError = " << track.dzError() << std::endl <<
        "chi^2/ndof = " << track.normalizedChi2() << std::endl;
  edm::LogInfo("Demo") << ss.str();
}

//define this as a plug-in
DEFINE_FWK_MODULE(TrackingEfficiencyFromCosmics);