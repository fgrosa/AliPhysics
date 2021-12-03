#include "AliAnalysisTaskCharmingFemto.h"

#include "yaml-cpp/yaml.h"

#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliMultSelection.h"
#include "AliPIDResponse.h"
#include "AliRDHFCutsDplustoKpipi.h"
#include "AliRDHFCutsDStartoKpipi.h"
#include "AliAODRecoDecayHF.h"
#include "AliAODRecoDecayHF3Prong.h"
#include "AliAODRecoCascadeHF.h"
#include "AliHFMLResponse.h"
#include "AliAODHandler.h"
#include "AliHFMLResponseDplustoKpipi.h"
#include "AliHFMLResponseDstartoD0pi.h"
#include "AliAnalysisTaskSECharmHadronMLSelector.h"
#include "AliMLModelHandler.h"
#include "TDatabasePDG.h"
#include "TRandom.h"

ClassImp(AliAnalysisTaskCharmingFemto)

    //____________________________________________________________________________________________________
    AliAnalysisTaskCharmingFemto::AliAnalysisTaskCharmingFemto()
    : AliAnalysisTaskSE("AliAnalysisTaskCharmingFemto"),
      fInputEvent(nullptr),
      fEvent(nullptr),
      fEvtCuts(nullptr),
      fProtonTrack(nullptr),
      fTrackCutsPartProton(nullptr),
      fTrackCutsPartAntiProton(nullptr),
      fConfig(nullptr),
      fPairCleaner(nullptr),
      fPartColl(nullptr),
      fIsMC(false),
      fIsMCtruth(false),
      fIsLightweight(false),
      fTrigger(AliVEvent::kINT7),
      fSystem(kpp13TeV),
      fCheckProtonSPDHit(false),
      fTrackBufferSize(2500),
      fDmesonPDGs{},
      fGTI(nullptr),
      fQA(nullptr),
      fEvtHistList(nullptr),
      fTrackCutHistList(nullptr),
      fTrackCutHistMCList(nullptr),
      fAntiTrackCutHistList(nullptr),
      fAntiTrackCutHistMCList(nullptr),
      fDChargedHistList(nullptr),
      fResultList(nullptr),
      fResultQAList(nullptr),
      fHistDplusInvMassPt(nullptr),
      fHistDplusInvMassPtSel(nullptr),
      fHistDplusEta(nullptr),
      fHistDplusPhi(nullptr),
      fHistDplusChildPt(),
      fHistDplusChildEta(),
      fHistDplusChildPhi(),
      fHistDplusMCPDGPt(nullptr),
      fHistDplusMCPtRes(nullptr),
      fHistDplusMCPhiRes(nullptr),
      fHistDplusMCThetaRes(nullptr),
      fHistDplusMCOrigin(nullptr),
      fHistDminusInvMassPt(nullptr),
      fHistDminusInvMassPtSel(nullptr),
      fHistDminusEta(nullptr),
      fHistDminusPhi(nullptr),
      fHistDminusChildPt(),
      fHistDminusChildEta(),
      fHistDminusChildPhi(),
      fHistDminusMCPDGPt(nullptr),
      fHistDminusMCPtRes(nullptr),
      fHistDminusMCPhiRes(nullptr),
      fHistDminusMCThetaRes(nullptr),
      fHistDminusMCOrigin(nullptr),
      fDecChannel(kDplustoKpipi),
      fRDHFCuts(nullptr),
      fAODProtection(0),
      fMassSelectionType(kSignal),
      fNSigmaMass(2.),
      fNSigmaOffsetSideband(5.),
      fLowerMassSelection(0.),
      fUpperMassSelection(999.),
      fSidebandWidth(0.2),
      fLowerDstarRemoval(1.992),
      fUpperDstarRemoval(2.028),
      fMCBeautyRejection(false),
      fMCBeautyScalingFactor(1.),
      fUseTrueDOnly(false),
      fInvMassCutLow(0.),
      fInvMassCutHigh(0.),
      fBuddypTlow(0.),
      fBuddypThigh(0.),
      fBuddyeta(0.),
      fBuddyOrigin(0),
      fDmesonOrigin(0),
      fApplyML(false),
      fConfigPath(""),
      fMLResponse(nullptr),
      fDependOnMLSelector(false),
      fPtLimsML{},
      fMLScoreCuts{},
      fMLOptScoreCuts{} {}

//____________________________________________________________________________________________________
AliAnalysisTaskCharmingFemto::AliAnalysisTaskCharmingFemto(const char *name,
                                                           const bool isMC,
                                                           const bool isMCtruth)
    : AliAnalysisTaskSE(name),
      fInputEvent(nullptr),
      fEvent(nullptr),
      fEvtCuts(nullptr),
      fProtonTrack(nullptr),
      fTrackCutsPartProton(nullptr),
      fTrackCutsPartAntiProton(nullptr),
      fConfig(nullptr),
      fPairCleaner(nullptr),
      fPartColl(nullptr),
      fIsMC(isMC),
      fIsMCtruth(isMCtruth),
      fIsLightweight(false),
      fTrigger(AliVEvent::kINT7),
      fSystem(kpp13TeV),
      fCheckProtonSPDHit(false),
      fTrackBufferSize(2500),
      fDmesonPDGs{},
      fGTI(nullptr),
      fQA(nullptr),
      fEvtHistList(nullptr),
      fTrackCutHistList(nullptr),
      fTrackCutHistMCList(nullptr),
      fAntiTrackCutHistList(nullptr),
      fAntiTrackCutHistMCList(nullptr),
      fDChargedHistList(nullptr),
      fResultList(nullptr),
      fResultQAList(nullptr),
      fHistDplusInvMassPt(nullptr),
      fHistDplusInvMassPtSel(nullptr),
      fHistDplusEta(nullptr),
      fHistDplusPhi(nullptr),
      fHistDplusChildPt(),
      fHistDplusChildEta(),
      fHistDplusChildPhi(),
      fHistDplusMCPDGPt(nullptr),
      fHistDminusInvMassPt(nullptr),
      fHistDminusInvMassPtSel(nullptr),
      fHistDplusMCPtRes(nullptr),
      fHistDplusMCPhiRes(nullptr),
      fHistDplusMCThetaRes(nullptr),
      fHistDplusMCOrigin(nullptr),
      fHistDminusEta(nullptr),
      fHistDminusPhi(nullptr),
      fHistDminusChildPt(),
      fHistDminusChildEta(),
      fHistDminusChildPhi(),
      fHistDminusMCPDGPt(nullptr),
      fHistDminusMCPtRes(nullptr),
      fHistDminusMCPhiRes(nullptr),
      fHistDminusMCThetaRes(nullptr),
      fHistDminusMCOrigin(nullptr),
      fDecChannel(kDplustoKpipi),
      fRDHFCuts(nullptr),
      fAODProtection(0),
      fMassSelectionType(kSignal),
      fNSigmaMass(2.),
      fNSigmaOffsetSideband(5.),
      fLowerMassSelection(0.),
      fUpperMassSelection(999.),
      fSidebandWidth(0.2),
      fLowerDstarRemoval(1.992),
      fUpperDstarRemoval(2.028),
      fMCBeautyRejection(false),
      fMCBeautyScalingFactor(1.),
      fUseTrueDOnly(false),
      fInvMassCutLow(0.),
      fInvMassCutHigh(0.),
      fBuddypTlow(0.),
      fBuddypThigh(0.),
      fBuddyeta(0.),
      fBuddyOrigin(0),
      fDmesonOrigin(0),
      fApplyML(false),
      fConfigPath(""),
      fMLResponse(nullptr),
      fDependOnMLSelector(false),
      fPtLimsML{},
      fMLScoreCuts{},
      fMLOptScoreCuts{} {
  DefineInput(0, TChain::Class());
  DefineOutput(1, TList::Class());
  DefineOutput(2, TList::Class());
  DefineOutput(3, TList::Class());
  DefineOutput(4, TList::Class());
  DefineOutput(5, TList::Class());
  DefineOutput(6, TList::Class());
  DefineOutput(7, TList::Class());
  switch(fDecChannel){ // save cut object for HF particle
    case kDplustoKpipi:
    {
      DefineOutput(8, AliRDHFCutsDplustoKpipi::Class());
      break;
    }
    case kDstartoKpipi:
    {
      DefineOutput(8, AliRDHFCutsDStartoKpipi::Class());
      break;
    }
    default:
    {
      AliFatal("Invalid HF channel.");
      break;
    }
  }

  if (fIsMC) {
    DefineOutput(9, TList::Class());
    DefineOutput(10, TList::Class());
  }
}

//____________________________________________________________________________________________________
AliAnalysisTaskCharmingFemto::~AliAnalysisTaskCharmingFemto() {
  delete fPartColl;
  delete fPairCleaner;
  delete fProtonTrack;
  delete fRDHFCuts;

  if(fApplyML && fMLResponse) {
    delete fMLResponse;
  }
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::LocalInit()
{
    // Initialization
    switch(fDecChannel) {
      case kDplustoKpipi:
      {
        AliRDHFCutsDplustoKpipi* copyCutDplus = new AliRDHFCutsDplustoKpipi(*(static_cast<AliRDHFCutsDplustoKpipi*>(fRDHFCuts)));
        PostData(8, copyCutDplus);
        break;
      }
      case kDstartoKpipi:
      {
        AliRDHFCutsDStartoKpipi* copyCutDstar = new AliRDHFCutsDStartoKpipi(*(static_cast<AliRDHFCutsDStartoKpipi*>(fRDHFCuts)));
        PostData(8, copyCutDstar);
        break;
      }
    }
  return;
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::UserExec(Option_t * /*option*/) {
  fInputEvent = static_cast<AliAODEvent*>(InputEvent());
  if (fIsMC)
    fMCEvent = MCEvent();

  // PREAMBLE - CHECK EVERYTHING IS THERE
  if (!fInputEvent) {
    AliError("No Input event");
    return;
  }

  // Protection against the mismatch of candidate TRefs:
  // Check if AOD and corresponding deltaAOD files contain the same number of events.
  // In case of discrepancy the event is rejected.
  if(fAODProtection >= 0) {
    // Protection against different number of events in the AOD and deltaAOD
    // In case of discrepancy the event is rejected.
    int matchingAODdeltaAODlevel = AliRDHFCuts::CheckMatchingAODdeltaAODevents();
    if (matchingAODdeltaAODlevel < 0 || (matchingAODdeltaAODlevel == 0 && fAODProtection == 1)) {
      // AOD/deltaAOD trees have different number of entries || TProcessID do not match while it was required
      return;
    }
  }

  // GET HF CANDIDATE ARRAY
  TClonesArray *arrayHF = nullptr;
  int absPdgMom = 0;
  TString mesonName = "";
  if(!fInputEvent && AODEvent() && IsStandardAOD()) {
    // In case there is an AOD handler writing a standard AOD, use the AOD
    // event in memory rather than the input (ESD) event.
    fInputEvent = dynamic_cast<AliAODEvent*>(AODEvent());
    // in this case the braches in the deltaAOD (AliAOD.VertexingHF.root)
    // have to taken from the AOD event hold by the AliAODExtension
    AliAODHandler* aodHandler = (AliAODHandler*)((AliAnalysisManager::GetAnalysisManager())->GetOutputEventHandler());
    if(aodHandler->GetExtensions()) {
      AliAODExtension *ext = (AliAODExtension*)aodHandler->GetExtensions()->FindObject("AliAOD.VertexingHF.root");
      AliAODEvent *aodFromExt = ext->GetAOD();
      switch(fDecChannel) {
        case kDplustoKpipi:
          absPdgMom = 411;
          mesonName = "Dplus";
          arrayHF = dynamic_cast<TClonesArray*>(aodFromExt->GetList()->FindObject("Charm3Prong"));
          break;
        case kDstartoKpipi:
          absPdgMom = 413;
          mesonName = "DStar";
          arrayHF = dynamic_cast<TClonesArray*>(aodFromExt->GetList()->FindObject("Dstar"));
          break;
      }
    }
  }
  else if(fInputEvent){
    switch(fDecChannel) {
      case kDplustoKpipi:
        absPdgMom = 411;
        mesonName = "Dplus";
        arrayHF = dynamic_cast<TClonesArray*>(fInputEvent->GetList()->FindObject("Charm3Prong"));
        break;
      case kDstartoKpipi:
        absPdgMom = 413;
        mesonName = "DStar";
        arrayHF = dynamic_cast<TClonesArray*>(fInputEvent->GetList()->FindObject("Dstar"));
        break;
    }
  }

  if(!arrayHF) {
    AliError("Branch not found!\n");
    return;
  }

  if (fIsMC && !fMCEvent) {
    AliError("No MC event");
    return;
  }

  if (!fEvtCuts) {
    AliError("Event Cuts missing");
    return;
  }

  if (!fTrackCutsPartProton || !fTrackCutsPartAntiProton) {
    AliError("Proton Cuts missing");
    return;
  }

  // Reset the pair cleaner
  fPairCleaner->ResetArray();

  // EVENT SELECTION
  fEvent->SetEvent(fInputEvent);
  if (!fEvtCuts->isSelected(fEvent))
    return;

  // PROTON SELECTION
  ResetGlobalTrackReference();
  for (int iTrack = 0; iTrack < fInputEvent->GetNumberOfTracks(); ++iTrack) {
    AliAODTrack *track = static_cast<AliAODTrack *>(fInputEvent->GetTrack(
        iTrack));
    if (!track) {
      AliFatal("No Standard AOD");
      return;
    }
    StoreGlobalTrackReference(track);
  }
  static std::vector<AliFemtoDreamBasePart> protons;
  static std::vector<AliFemtoDreamBasePart> antiprotons;
  static std::vector<AliFemtoDreamBasePart> pions;
  static std::vector<AliFemtoDreamBasePart> antipions;
  static std::vector<AliFemtoDreamBasePart> kaons;
  static std::vector<AliFemtoDreamBasePart> antikaons;
  static std::vector<AliFemtoDreamBasePart> dplus;
  static std::vector<AliFemtoDreamBasePart> dminus;
  protons.clear();
  antiprotons.clear();
  pions.clear();
  antipions.clear();
  kaons.clear();
  antikaons.clear();
  dplus.clear();
  dminus.clear();

  const int multiplicity = fEvent->GetMultiplicity();
  fProtonTrack->SetGlobalTrackInfo(fGTI, fTrackBufferSize);
  for (int iTrack = 0; iTrack < fInputEvent->GetNumberOfTracks(); ++iTrack) {
    AliAODTrack *track = static_cast<AliAODTrack *>(fInputEvent->GetTrack(
        iTrack));

    if (fCheckProtonSPDHit) {
      if (!track->HasPointOnITSLayer(0) && !track->HasPointOnITSLayer(1)) {
        continue;
      }
    }

    float DmesonBuddyMass =
      TDatabasePDG::Instance()->GetParticle(fTrackCutsPartProton->GetPDGCode())->Mass();

    fProtonTrack->SetTrack(track);
    fProtonTrack->SetInvMass(DmesonBuddyMass);
    if (fTrackCutsPartProton->isSelected(fProtonTrack)) {
      if (!fIsMCtruth) {
        protons.push_back(*fProtonTrack);
      }
    }
    if (fTrackCutsPartAntiProton->isSelected(fProtonTrack)) {
      if (!fIsMCtruth) {
        antiprotons.push_back(*fProtonTrack);
      }
    }
  }

  if (fIsMCtruth) {
    TClonesArray *fArrayMCAOD = dynamic_cast<TClonesArray *>(
        fInputEvent->FindListObject(AliAODMCParticle::StdBranchName()));
    int noPart = fArrayMCAOD->GetEntriesFast();
    auto trackCutsDdau = fRDHFCuts->GetTrackCuts();
    float ptMin, ptMax, etaMin, etaMax;
    trackCutsDdau->GetPtRange(ptMin, ptMax);
    trackCutsDdau->GetEtaRange(etaMin, etaMax);
    float* DmesonPtBins = fRDHFCuts->GetPtBinLimits();
    //for (Int_t i=0;i<fRDHFCuts->GetNPtBins();i++) cout<<i<<" "<<DmesonPtBins[i]<<endl;
    float DmesonPtMin=DmesonPtBins[0];
    float DmesonPtMax=DmesonPtBins[fRDHFCuts->GetNPtBins()];
  
    for (int iPart = 1; iPart < noPart; iPart++) {
      AliAODMCParticle *mcPart = (AliAODMCParticle *)fArrayMCAOD->At(iPart);
      if (!(mcPart)) {
        std::cout << "NO MC particle" << std::endl;
        continue;
      }

      if (mcPart->GetLabel() < 0) {
        continue;
      }

      AliFemtoDreamBasePart part;

      int mcpdg = mcPart->GetPdgCode();
      double pt = mcPart->Pt();
      double eta = mcPart->Eta();
      if (mcpdg == fTrackCutsPartProton->GetPDGCode()) {        
        if ((pt < fBuddypThigh && pt > fBuddypTlow) && (eta > -fBuddyeta && eta < fBuddyeta)) {
          if(SelectBuddyOrigin(mcPart)){
            part.SetMCParticleRePart(mcPart);
            part.SetID(mcPart->GetLabel());
            part.SetIDTracks(mcPart->GetLabel());
            protons.push_back(part);
          }
        }
      }

      if (mcpdg == fTrackCutsPartAntiProton->GetPDGCode()) {
        if ((pt < fBuddypThigh && pt > fBuddypTlow) && (eta > -fBuddyeta && eta < fBuddyeta)) {
          if(SelectBuddyOrigin(mcPart)){
            part.SetMCParticleRePart(mcPart);
            part.SetID(mcPart->GetLabel());
            part.SetIDTracks(mcPart->GetLabel());
            antiprotons.push_back(part);
          }
        }
      }

      if (mcpdg == 321) {
        if ((pt < ptMax && pt > ptMin) && (eta > etaMin && eta < etaMax)) {
          part.SetMCParticleRePart(mcPart);
          part.SetID(mcPart->GetLabel());
          // cout<<iPart<<" "<<mcPart->GetLabel()<<" "<<part.GetID()<<endl;
          part.SetPDGCode(mcPart->GetPdgCode());

          int motherID = mcPart->GetMother();

          if (motherID <= noPart) {
            AliAODMCParticle *mcMother = nullptr;

            mcMother = (AliAODMCParticle *)fArrayMCAOD->At(motherID);

            if (mcMother) {
              if (mcMother->GetLabel() < 0) {
                continue;
              }
              part.SetMotherID(motherID);
              part.SetMotherPDG(mcMother->GetPdgCode());

            } else {
              part.SetMotherID(0);
              part.SetMotherPDG(0);
            }
            
            kaons.push_back(part);
          }
        }
      }

      if (mcpdg == -321) {
        if ((pt < ptMax && pt > ptMin) && (eta > etaMin && eta < etaMax)) {
          part.SetMCParticleRePart(mcPart);
          part.SetID(mcPart->GetLabel());
          part.SetPDGCode(mcPart->GetPdgCode());
          int motherID = mcPart->GetMother();

          if (motherID <= noPart) {
            AliAODMCParticle *mcMother = nullptr;

            mcMother = (AliAODMCParticle *)fArrayMCAOD->At(motherID);

            if (mcMother) {
              if (mcMother->GetLabel() < 0) {
                continue;
              }
              part.SetMotherID(motherID);
              part.SetMotherPDG(mcMother->GetPdgCode());

            } else {
              part.SetMotherID(0);
              part.SetMotherPDG(0);
            }

            antikaons.push_back(part);
          }
        }
      }
    

      if (mcpdg == 211) {
        if ((pt < ptMax && pt > ptMin) && (eta > etaMin && eta < etaMax)) {
          part.SetMCParticleRePart(mcPart);
          part.SetID(mcPart->GetLabel());
          part.SetPDGCode(mcPart->GetPdgCode());

          int motherID = mcPart->GetMother();

          if (motherID <= noPart) {
            AliAODMCParticle *mcMother = nullptr;

            mcMother = (AliAODMCParticle *)fArrayMCAOD->At(motherID);

            if (mcMother) {
              if (mcMother->GetLabel() < 0) {
                continue;
              }
              part.SetMotherID(motherID);
              part.SetMotherPDG(mcMother->GetPdgCode());

            } else {
              part.SetMotherID(0);
              part.SetMotherPDG(0);
            }

            pions.push_back(part);
          }
        }
      }

      if (mcpdg == -211) {
        if ((pt < ptMax && pt > ptMin) && (eta > etaMin && eta < etaMax)) {
          part.SetMCParticleRePart(mcPart);
          part.SetID(mcPart->GetLabel());
          part.SetPDGCode(mcPart->GetPdgCode());
          int motherID = mcPart->GetMother();

          if (motherID <= noPart) {
            AliAODMCParticle *mcMother = nullptr;

            mcMother = (AliAODMCParticle *)fArrayMCAOD->At(motherID);

            if (mcMother) {
              if (mcMother->GetLabel() < 0) {
                continue;
              }
              part.SetMotherID(motherID);
              part.SetMotherPDG(mcMother->GetPdgCode());

            } else {
              part.SetMotherID(0);
              part.SetMotherPDG(0);
            }
            
            antipions.push_back(part);
          }
        }
      }


    }

    for (const auto &posPi1Daughter : pions) {
      int mcpdgm1 = posPi1Daughter.GetMotherPDG();
      int motherIDPip1 = posPi1Daughter.GetMotherID();
      int Pi1ID = posPi1Daughter.GetID();

      for (const auto &posPi2Daughter : pions) {
        int mcpdgm2 = posPi2Daughter.GetMotherPDG();
        int motherIDPip2 = posPi2Daughter.GetMotherID();
        int Pi2ID = posPi2Daughter.GetID();

        for (const auto &negKDaughter : antikaons) {
        int mcpdgm3 = negKDaughter.GetMotherPDG();
        int motherIDKm = negKDaughter.GetMotherID();

          if(Pi1ID != Pi2ID) {

            float Pi1P[3], Pi2P[3], KP[3];
            posPi1Daughter.GetMomentum().GetXYZ(Pi1P);
            posPi2Daughter.GetMomentum().GetXYZ(Pi2P);
            negKDaughter.GetMomentum().GetXYZ(KP);

            TLorentzVector trackPi1, trackPi2, trackK;
            float Kaonmass = TDatabasePDG::Instance()->GetParticle(321)->Mass();
            float Pionmass = TDatabasePDG::Instance()->GetParticle(211)->Mass();

            trackPi1.SetXYZM(Pi1P[0], Pi1P[1], Pi1P[2], Pionmass);
            trackPi2.SetXYZM(Pi2P[0], Pi2P[1], Pi2P[2], Pionmass);
            trackK.SetXYZM(KP[0], KP[1], KP[2], Kaonmass);

            TLorentzVector trackSum = trackPi1 + trackPi2 + trackK;
            if ((trackSum.M() < fUpperMassSelection) &&
                (trackSum.M() > fLowerMassSelection)) {
              // cout << trackSum.M() <<" "<< mcpdgm1<<" "<< mcpdgm2 <<" "<< mcpdgm3 << endl;
              if (((motherIDPip1 == motherIDPip2) && (motherIDPip1 == motherIDKm))) {
                if ((mcpdgm1 == 411) && (mcpdgm2 == 411) && (mcpdgm3 == 411)) {
                  // cout<<"DPLUSUSUSU"<<endl;
                  AliAODMCParticle *mcMother = nullptr;
                  mcMother = (AliAODMCParticle *)fArrayMCAOD->At(motherIDPip1);
                  if((mcMother->Pt()>DmesonPtMin) && (mcMother->Pt()<DmesonPtMax)) {
                    // AliFemtoDreamBasePart V0part;
                    // V0part.SetMCParticleRePart(mcMother);
                    // V0part.SetIDTracks(Pi1ID);
                    // V0part.SetIDTracks(Pi2ID);
                    // V0part.SetIDTracks(negKDaughter.GetID());
                    // dplus.push_back(V0part);
                    //int dmesonorigin=AliVertexingHFUtils::CheckOrigin(fArrayMCAOD, mcMother, false);
                    if (SelectBuddyOrigin(fArrayMCAOD, mcMother)){
                      AliFemtoDreamBasePart V0part;
                      V0part.SetMCParticleRePart(mcMother);
                      V0part.SetIDTracks(Pi1ID);
                      V0part.SetIDTracks(Pi2ID);
                      V0part.SetIDTracks(negKDaughter.GetID());
                      dplus.push_back(V0part);
                    }
                  }
                }
              }
            }
          }
        }          
      }
    }

    for (const auto &negPi1Daughter : antipions) {
      int mcpdgm1 = negPi1Daughter.GetMotherPDG();
      int motherIDPip1 = negPi1Daughter.GetMotherID();
      int Pi1ID = negPi1Daughter.GetID();

      for (const auto &negPi2Daughter : antipions) {
        int mcpdgm2 = negPi2Daughter.GetMotherPDG();
        int motherIDPip2 = negPi2Daughter.GetMotherID();
        int Pi2ID = negPi2Daughter.GetID();

        for (const auto &posKDaughter : kaons) {
        int mcpdgm3 = posKDaughter.GetMotherPDG();
        int motherIDKm = posKDaughter.GetMotherID();

          if(Pi1ID != Pi2ID) {

            float Pi1P[3], Pi2P[3], KP[3];
            negPi1Daughter.GetMomentum().GetXYZ(Pi1P);
            negPi2Daughter.GetMomentum().GetXYZ(Pi2P);
            posKDaughter.GetMomentum().GetXYZ(KP);

            TLorentzVector trackPi1, trackPi2, trackK;
            float Kaonmass = TDatabasePDG::Instance()->GetParticle(321)->Mass();
            float Pionmass = TDatabasePDG::Instance()->GetParticle(211)->Mass();

            trackPi1.SetXYZM(Pi1P[0], Pi1P[1], Pi1P[2], Pionmass);
            trackPi2.SetXYZM(Pi2P[0], Pi2P[1], Pi2P[2], Pionmass);
            trackK.SetXYZM(KP[0], KP[1], KP[2], Kaonmass);

            TLorentzVector trackSum = trackPi1 + trackPi2 + trackK;
            if ((trackSum.M() < fUpperMassSelection) &&
                (trackSum.M() > fLowerMassSelection)) {
              if (((motherIDPip1 == motherIDPip2) && (motherIDPip1 == motherIDKm))) {
                if ((mcpdgm1 == -411) && (mcpdgm2 == -411) && (mcpdgm3 == -411)) {
                  // cout << trackSum.M() << endl;
                  AliAODMCParticle *mcMother = nullptr;
                  mcMother = (AliAODMCParticle *)fArrayMCAOD->At(motherIDPip1);
                  if((mcMother->Pt()>DmesonPtMin) && (mcMother->Pt()<DmesonPtMax)) {
                    // AliFemtoDreamBasePart V0part;
                    // V0part.SetMCParticleRePart(mcMother);
                    // V0part.SetIDTracks(Pi1ID);
                    // V0part.SetIDTracks(Pi2ID);
                    // V0part.SetIDTracks(posKDaughter.GetID());
                    // std::vector<int> IDDaug = V0part.GetIDTracks();
                    //   for (auto itIDs = IDDaug.begin(); itIDs != IDDaug.end(); ++itIDs) {
                    //     std::cout <<" IDs of Daughter: "<<*itIDs<<'\n';
                    //   }                                    
                    // dminus.push_back(V0part);

                    if (SelectBuddyOrigin(fArrayMCAOD, mcMother)){
                      AliFemtoDreamBasePart V0part;
                      V0part.SetMCParticleRePart(mcMother);
                      V0part.SetIDTracks(Pi1ID);
                      V0part.SetIDTracks(Pi2ID);
                      V0part.SetIDTracks(posKDaughter.GetID());
                      dminus.push_back(V0part);
                    }
                  }
                }
              }
            }
          }
        }          
      }
    }


  }

  // std::cout << "MC truth DONE: "<<inili << std::endl;
  // D MESON SELECTION
  int nCand = arrayHF->GetEntriesFast();

  // check if the train includes the common ML selector for the given charm-hadron species
  AliAnalysisTaskSECharmHadronMLSelector *taskMLSelect = nullptr;
  std::vector<int> chHadIdx{};
  std::vector<std::vector<double> > scoresFromMLSelector{};
  if(fDependOnMLSelector) {
    taskMLSelect = dynamic_cast<AliAnalysisTaskSECharmHadronMLSelector*>(AliAnalysisManager::GetAnalysisManager()->GetTask(Form("MLSelector%s", mesonName.Data())));
    if(!taskMLSelect) {
      AliFatal("ML Selector not present in train and ML models not compiled!");
      return;
    }

    chHadIdx = taskMLSelect->GetSelectedCandidates();
    scoresFromMLSelector = taskMLSelect->GetMLSCores();
  }
  else {
    for (int iCand = 0; iCand < nCand; iCand++) {
      chHadIdx.push_back(iCand);
      scoresFromMLSelector.push_back({});
    }
  }

  // needed to initialise PID response
  fRDHFCuts->IsEventSelected(fInputEvent);

  AliAODRecoDecayHF *dMeson = nullptr;
  AliAODRecoDecayHF *dMesonWithVtx = nullptr;
  for (size_t iCand = 0; iCand < chHadIdx.size(); iCand++) {

    dMeson = dynamic_cast<AliAODRecoDecayHF*>(arrayHF->UncheckedAt(chHadIdx[iCand]));
    if(fDecChannel == kDstartoKpipi){
      dMesonWithVtx = dynamic_cast<AliAODRecoDecayHF*>(((AliAODRecoCascadeHF *)dMeson)->Get2Prong());
    } else {
      dMesonWithVtx = dMeson;
    }

    bool unsetVtx = false;
    bool recVtx = false;
    AliAODVertex *origOwnVtx = nullptr;

    int isSelected = IsCandidateSelected(dMeson, dMesonWithVtx, absPdgMom, unsetVtx, recVtx, origOwnVtx, scoresFromMLSelector[iCand]);
    if(!isSelected) {
      if (unsetVtx) {
        dMesonWithVtx->UnsetOwnPrimaryVtx();
      }
      if (recVtx) {
        fRDHFCuts->CleanOwnPrimaryVtx(dMesonWithVtx, fInputEvent, origOwnVtx);
      }
      continue;
    }

    double mass;
    if(fDecChannel != kDstartoKpipi) {
      mass = dMeson->InvMass(fDmesonPDGs.size(), &fDmesonPDGs[0]);
    } else {
      mass = dynamic_cast<AliAODRecoCascadeHF *>(dMeson)->DeltaInvMass();
    }

    if (dMeson->Charge() > 0) {
      fHistDplusInvMassPt->Fill(dMeson->Pt(), mass);
    } else {
      fHistDminusInvMassPt->Fill(dMeson->Pt(), mass);
    }


    if( MassSelection(mass, dMeson->Pt(), absPdgMom) ) {
      if (dMeson->Charge() > 0) {
        AliFemtoDreamBasePart dplusCand(dMeson, fInputEvent, absPdgMom, fDmesonPDGs);
        if (fIsMC && fMCBeautyRejection
            && dplusCand.GetParticleOrigin()
                == AliFemtoDreamBasePart::kBeauty) {
          if (gRandom->Uniform() > fMCBeautyScalingFactor)
            continue;
        }
        if (fIsMC && fUseTrueDOnly
            && std::abs(dplusCand.GetMCPDGCode()) != absPdgMom) {
          continue;
        }
        if (!fIsMCtruth) {
          dplus.push_back(dplusCand);
        }
        if (!fIsLightweight) {
          fHistDplusInvMassPtSel->Fill(dMeson->Pt(), mass);
          fHistDplusEta->Fill(dMeson->Eta());
          fHistDplusPhi->Fill(dMeson->Phi());
          if (fIsMC) {
            fHistDplusMCPDGPt->Fill(dplusCand.GetPt(),
                                    dplusCand.GetMotherPDG());
            fHistDplusMCOrigin->Fill(dplusCand.GetPt(),
                                     dplusCand.GetParticleOrigin());
            if (dplusCand.GetMCPDGCode() != 0) {
              fHistDplusMCPtRes->Fill(dplusCand.GetPt() - dplusCand.GetMCPt(),
                                      dplusCand.GetPt());
              fHistDplusMCPhiRes->Fill(
                  dplusCand.GetPhi().at(0) - dplusCand.GetMCPhi().at(0),
                  dplusCand.GetPt());
              fHistDplusMCThetaRes->Fill(
                  dplusCand.GetTheta().at(0) - dplusCand.GetMCTheta().at(0),
                  dplusCand.GetPt());
            }
          }
          for (unsigned int iChild = 0; iChild < fDmesonPDGs.size(); iChild++) {
            AliAODTrack *track;
            if (fDecChannel != kDstartoKpipi || iChild == 0) {
              track = (AliAODTrack *) dMeson->GetDaughter(iChild);
            } else {
              track = (AliAODTrack *) dMesonWithVtx->GetDaughter(iChild-1); //D0<-D* daughters
            }
            fHistDplusChildPt[iChild]->Fill(track->Pt());
            fHistDplusChildEta[iChild]->Fill(track->Eta());
            fHistDplusChildPhi[iChild]->Fill(track->Phi());
          }
        }
      } else {
        AliFemtoDreamBasePart dminusCand(dMeson, fInputEvent, absPdgMom,
                                         fDmesonPDGs);
        if (fIsMC && fMCBeautyRejection
            && dminusCand.GetParticleOrigin()
                == AliFemtoDreamBasePart::kBeauty) {
          if (gRandom->Uniform() > fMCBeautyScalingFactor)
            continue;
        }
        if (fIsMC && fUseTrueDOnly
            && std::abs(dminusCand.GetMCPDGCode()) != absPdgMom) {
          continue;
        }
        if (!fIsMCtruth){
          dminus.push_back(dminusCand);
        }
        if (!fIsLightweight) {
          fHistDminusInvMassPtSel->Fill(dMeson->Pt(), mass);
          fHistDminusEta->Fill(dMeson->Eta());
          fHistDminusPhi->Fill(dMeson->Phi());
          if (fIsMC) {
            fHistDminusMCPDGPt->Fill(dminusCand.GetPt(),
                                     dminusCand.GetMotherPDG());
            fHistDminusMCOrigin->Fill(dminusCand.GetPt(),
                                      dminusCand.GetParticleOrigin());
            if (dminusCand.GetMCPDGCode() != 0) {
              fHistDminusMCPtRes->Fill(
                  dminusCand.GetPt() - dminusCand.GetMCPt(),
                  dminusCand.GetPt());
              fHistDminusMCPhiRes->Fill(
                  dminusCand.GetPhi().at(0) - dminusCand.GetMCPhi().at(0),
                  dminusCand.GetPt());
              fHistDminusMCThetaRes->Fill(
                  dminusCand.GetTheta().at(0) - dminusCand.GetMCTheta().at(0),
                  dminusCand.GetPt());
            }
          }
          for (unsigned int iChild = 0; iChild < fDmesonPDGs.size(); iChild++) {
            AliAODTrack *track;
            if (fDecChannel != kDstartoKpipi || iChild == 0) {
              track = (AliAODTrack *) dMeson->GetDaughter(iChild);
            } else {
              track = (AliAODTrack *) dMesonWithVtx->GetDaughter(iChild-1); //D0<-D* daughters
            }
            fHistDminusChildPt[iChild]->Fill(track->Pt());
            fHistDminusChildEta[iChild]->Fill(track->Eta());
            fHistDminusChildPhi[iChild]->Fill(track->Phi());
          }
        }
      }
    }

    if (unsetVtx) {
      dMeson->UnsetOwnPrimaryVtx();
    }
    if (recVtx) {
      fRDHFCuts->CleanOwnPrimaryVtx(dMeson, fInputEvent, origOwnVtx);
    }
  }

  // PAIR CLEANING AND FEMTO
  fPairCleaner->CleanTrackAndDecay(&protons, &dplus, 0);
  fPairCleaner->CleanTrackAndDecay(&protons, &dminus, 1);
  fPairCleaner->CleanTrackAndDecay(&antiprotons, &dplus, 2);
  fPairCleaner->CleanTrackAndDecay(&antiprotons, &dminus, 3);

  fPairCleaner->StoreParticle(protons);
  fPairCleaner->StoreParticle(antiprotons);

  fPairCleaner->StoreParticle(dplus);
  fPairCleaner->StoreParticle(dminus);

  fPartColl->SetEvent(fPairCleaner->GetCleanParticles(), fEvent);

  // flush the data
  PostData(1, fQA);
  PostData(2, fEvtHistList);
  PostData(3, fTrackCutHistList);
  PostData(4, fAntiTrackCutHistList);
  PostData(5, fDChargedHistList);
  PostData(6, fResultList);
  PostData(7, fResultQAList);
  if (fIsMC) {
    PostData(9, fTrackCutHistMCList);
    PostData(10, fAntiTrackCutHistMCList);
  }
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::ResetGlobalTrackReference() {
  // see AliFemtoDreamAnalysis for details
  for (int i = 0; i < fTrackBufferSize; i++) {
    fGTI[i] = 0;
  }
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::StoreGlobalTrackReference(
    AliAODTrack *track) {
  const int trackID = track->GetID();
  if (trackID < 0) {
    return;
  }

  if (trackID >= fTrackBufferSize) {
    printf("Warning: track ID too big for buffer: ID: %d, buffer %d\n", trackID,
           fTrackBufferSize);
    return;
  }

  if (fGTI[trackID]) {
    if ((!track->GetFilterMap()) && (!track->GetTPCNcls())) {
      return;
    }
    if (fGTI[trackID]->GetFilterMap() || fGTI[trackID]->GetTPCNcls()) {
      printf("Warning! global track info already there!");
      printf("         TPCNcls track1 %u track2 %u",
             (fGTI[trackID])->GetTPCNcls(), track->GetTPCNcls());
      printf("         FilterMap track1 %u track2 %u\n",
             (fGTI[trackID])->GetFilterMap(), track->GetFilterMap());
    }
  }
  (fGTI[trackID]) = track;
}

//____________________________________________________________________________________________________
void AliAnalysisTaskCharmingFemto::UserCreateOutputObjects() {
  fGTI = new AliAODTrack *[fTrackBufferSize];

  fEvent = new AliFemtoDreamEvent(true, !fIsLightweight, fTrigger);

  fProtonTrack = new AliFemtoDreamTrack();
  fProtonTrack->SetUseMCInfo(fIsMC);

  fPairCleaner = new AliFemtoDreamPairCleaner(4, 0,
                                              fConfig->GetMinimalBookingME());
  fPartColl = new AliFemtoDreamPartCollection(fConfig,
                                              fConfig->GetMinimalBookingME());

  fQA = new TList();
  fQA->SetName("QA");
  fQA->SetOwner(kTRUE);

  if (fEvtCuts) {
    fEvtCuts->InitQA();
    if (fEvent->GetEvtCutList() && !fIsLightweight) {
      fQA->Add(fEvent->GetEvtCutList());
    }
    if (fEvtCuts->GetHistList() && !fIsLightweight) {
      fEvtHistList = fEvtCuts->GetHistList();
    } else {
      fEvtHistList = new TList();
      fEvtHistList->SetName("EvtCuts");
      fEvtHistList->SetOwner(true);
    }
  } else {
    AliWarning("Event cuts are missing! \n");
  }

  if (!fConfig->GetMinimalBookingME() && fPairCleaner
      && fPairCleaner->GetHistList()) {
    fQA->Add(fPairCleaner->GetHistList());
  }

  fTrackCutsPartProton->Init("Proton");
  // necessary for the non-min booking case
  fTrackCutsPartProton->SetName("Proton");

  if (fTrackCutsPartProton && fTrackCutsPartProton->GetQAHists()) {
    fTrackCutHistList = fTrackCutsPartProton->GetQAHists();
    if (fIsMC && fTrackCutsPartProton->GetMCQAHists()
        && fTrackCutsPartProton->GetIsMonteCarlo()) {
      fTrackCutHistMCList = fTrackCutsPartProton->GetMCQAHists();
    }
  }

  fTrackCutsPartAntiProton->Init("Anti-proton");
  // necessary for the non-min booking case
  fTrackCutsPartAntiProton->SetName("Anti-proton");

  if (fTrackCutsPartAntiProton && fTrackCutsPartAntiProton->GetQAHists()) {
    fAntiTrackCutHistList = fTrackCutsPartAntiProton->GetQAHists();
    if (fIsMC && fTrackCutsPartAntiProton->GetMCQAHists()
        && fTrackCutsPartAntiProton->GetIsMonteCarlo()) {
      fAntiTrackCutHistMCList = fTrackCutsPartAntiProton->GetMCQAHists();
    }
  }

  // Eventually we might put this in a separate class but for the moment let's just leave it floating around here
  fDChargedHistList  = new TList();
  fDChargedHistList->SetName("DChargedQA");
  fDChargedHistList->SetOwner(true);

  TString nameD;
  TString nameDminus;
  TString nameInvMass;
  Int_t InvMassBins;
  Double_t LowerInvMass;
  Double_t UpperInvMass;
  switch (fDecChannel) {
    case kDplustoKpipi:
      nameD = "Dplus";
      nameDminus = "Dminus";
      nameInvMass = "#it{M}_{K#pi#pi}";
      InvMassBins = 100;
      LowerInvMass = 1.77;
      UpperInvMass = 1.97;
      break;
    case kDstartoKpipi:
      nameD = "Dstar";
      nameDminus = "Dstarminus";
      nameInvMass = "#it{M}_{K#pi#pi} - #it{M}(K#pi)";
      InvMassBins = 500;
      LowerInvMass = 0.138;
      UpperInvMass = 0.160;
      break;
  }

  fHistDplusInvMassPt = new TH2F(
      TString::Format("fHist%sInvMassPt", nameD.Data()),
      TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
      100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
  fDChargedHistList->Add(fHistDplusInvMassPt);
  if (!fIsLightweight) {
    fHistDplusInvMassPtSel = new TH2F(
        TString::Format("fHist%sInvMassPtSel", nameD.Data()),
        TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
        100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
    fDChargedHistList->Add(fHistDplusInvMassPtSel);
    fHistDplusEta = new TH1F(TString::Format("fHist%sEta", nameD.Data()),
                             ";#eta; Entries", 100, -1, 1);
    fDChargedHistList->Add(fHistDplusEta);
    fHistDplusPhi = new TH1F(TString::Format("fHist%sPhi", nameD.Data()),
                             ";#phi; Entries", 100, 0., 2. * TMath::Pi());
    fDChargedHistList->Add(fHistDplusPhi);

    if (fIsMC) {
      fHistDplusMCPDGPt = new TH2F(
          TString::Format("fHist%sMCPDGPt", nameD.Data()),
          "; #it{p}_{T} (GeV/#it{c}); PDG Code mother",
          250, 0, 25, 5000, 0, 5000);
      fHistDplusMCPtRes = new TH2F(
          TString::Format("fHist%sMCPtRes", nameD.Data()),
          "; #it{p}_{T, rec} - #it{p}_{T, gen} (GeV/#it{c}); #it{p}_{T} (GeV/#it{c})",
          101, -0.5, 0.5, 100, 0, 10);
      fHistDplusMCPhiRes = new TH2F(
          TString::Format("fHist%sMCPhiRes", nameD.Data()),
          "; #phi_{rec} - #phi_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025, 0.025,
          100, 0, 10);
      fHistDplusMCThetaRes = new TH2F(
          TString::Format("fHist%sMCThetaRes", nameD.Data()),
          "; #theta_{rec} - #theta_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025,
          0.025, 100, 0, 10);
      fHistDplusMCOrigin = new TH2F(TString::Format("fHist%sMCOrigin", nameD.Data()),
                                    "; #it{p}_{T} (GeV/#it{c}); Origin", 100, 0,
                                    10, 7, 0, 7);
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(1, "kPhysPrimary");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(2, "kWeak");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(3, "kMaterial");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(4, "kFake");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(5, "kContamination");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(6, "kUnknown");
      fHistDplusMCOrigin->GetYaxis()->SetBinLabel(7, "kBeauty");
      fDChargedHistList->Add(fHistDplusMCPDGPt);
      fDChargedHistList->Add(fHistDplusMCPtRes);
      fDChargedHistList->Add(fHistDplusMCPhiRes);
      fDChargedHistList->Add(fHistDplusMCThetaRes);
      fDChargedHistList->Add(fHistDplusMCOrigin);
    }
  }

  fHistDminusInvMassPt = new TH2F(
      TString::Format("fHist%sInvMassPt", nameDminus.Data()),
      TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
      100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
  fDChargedHistList->Add(fHistDminusInvMassPt);
  if (!fIsLightweight) {
    fHistDminusInvMassPtSel = new TH2F(
        TString::Format("fHist%sInvMassPtSel", nameDminus.Data()),
        TString::Format("; #it{p}_{T} (GeV/#it{c}); %s (GeV/#it{c}^{2})", nameInvMass.Data()),
        100, 0, 10, InvMassBins, LowerInvMass, UpperInvMass);
    fDChargedHistList->Add(fHistDminusInvMassPtSel);
    fHistDminusEta = new TH1F(TString::Format("fHist%sEta", nameDminus.Data()),
                              ";#eta; Entries", 100, -1, 1);
    fDChargedHistList->Add(fHistDminusEta);
    fHistDminusPhi = new TH1F(TString::Format("fHist%sPhi", nameDminus.Data()),
                              ";#phi; Entries", 100, 0., 2. * TMath::Pi());
    fDChargedHistList->Add(fHistDminusPhi);

    if (fIsMC) {
      fHistDminusMCPDGPt = new TH2F(
          TString::Format("fHist%sMCPDGPt", nameDminus.Data()),
          "; #it{p}_{T} (GeV/#it{c}); PDG Code mother",
          250, 0, 25, 5000, 0, 5000);
      fHistDminusMCPtRes = new TH2F(
          TString::Format("fHist%sMCPtRes", nameDminus.Data()),
          "; #it{p}_{T, rec} - #it{p}_{T, gen} (GeV/#it{c}); #it{p}_{T} (GeV/#it{c})",
          101, -0.5, 0.5, 100, 0, 10);
      fHistDminusMCPhiRes = new TH2F(
          TString::Format("fHist%sMCPhiRes", nameDminus.Data()),
          "; #phi_{rec} - #phi_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025, 0.025,
          100, 0, 10);
      fHistDminusMCThetaRes = new TH2F(
          TString::Format("fHist%sMCThetaRes", nameDminus.Data()),
          "; #theta_{rec} - #theta_{gen}; #it{p}_{T} (GeV/#it{c})", 101, -0.025,
          0.025, 100, 0, 10);
      fHistDminusMCOrigin = new TH2F(TString::Format("fHist%sMCOrigin", nameDminus.Data()),
                                     "; #it{p}_{T} (GeV/#it{c}); Origin", 100,
                                     0, 10, 7, 0, 7);
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(1, "kPhysPrimary");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(2, "kWeak");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(3, "kMaterial");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(4, "kFake");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(5, "kContamination");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(6, "kUnknown");
      fHistDminusMCOrigin->GetYaxis()->SetBinLabel(7, "kBeauty");
      fDChargedHistList->Add(fHistDminusMCPDGPt);
      fDChargedHistList->Add(fHistDminusMCPtRes);
      fDChargedHistList->Add(fHistDminusMCPhiRes);
      fDChargedHistList->Add(fHistDminusMCThetaRes);
      fDChargedHistList->Add(fHistDminusMCOrigin);
    }
  }


  if (!fIsLightweight) {
    std::vector<TString> nameVec;
    if (fDecChannel == kDplustoKpipi) {
      nameVec = { {"K", "Pi1", "Pi2"}};
    } else if (fDecChannel == kDstartoKpipi) {
      nameVec = { {"Pi1", "K", "Pi2"}};
    }
    for (unsigned int iChild = 0; iChild < fDmesonPDGs.size(); ++iChild) {
      fHistDplusChildPt[iChild] = new TH1F(
          TString::Format("fHist%sChildPt_%s", nameD.Data(), nameVec.at(iChild).Data()),
          "; #it{p}_{T} (GeV/#it{c}); Entries", 250, 0, 25);
      fHistDplusChildEta[iChild] = new TH1F(
          TString::Format("fHist%sChildEta_%s", nameD.Data(), nameVec.at(iChild).Data()),
          "; #eta; Entries", 100, -1, 1);
      fHistDplusChildPhi[iChild] = new TH1F(
          TString::Format("fHist%sChildPhi_%s", nameD.Data(), nameVec.at(iChild).Data()),
          "; #phi; Entries", 100, 0, 2. * TMath::Pi());
      fDChargedHistList->Add(fHistDplusChildPt[iChild]);
      fDChargedHistList->Add(fHistDplusChildEta[iChild]);
      fDChargedHistList->Add(fHistDplusChildPhi[iChild]);
      fHistDminusChildPt[iChild] = new TH1F(
          TString::Format("fHist%sChildPt_%s", nameDminus.Data(), nameVec.at(iChild).Data()),
          "; #it{p}_{T} (GeV/#it{c}); Entries", 250, 0, 25);
      fHistDminusChildEta[iChild] = new TH1F(
          TString::Format("fHist%sChildEta_%s", nameDminus.Data(), nameVec.at(iChild).Data()),
          "; #eta; Entries", 100, -1, 1);
      fHistDminusChildPhi[iChild] = new TH1F(
          TString::Format("fHist%sChildPhi_%s", nameDminus.Data(), nameVec.at(iChild).Data()),
          "; #phi; Entries", 100, 0, 2. * TMath::Pi());
      fDChargedHistList->Add(fHistDminusChildPt[iChild]);
      fDChargedHistList->Add(fHistDminusChildEta[iChild]);
      fDChargedHistList->Add(fHistDminusChildPhi[iChild]);
    }
  }


  if (fPartColl && fPartColl->GetHistList()) {
    fResultList = fPartColl->GetHistList();
  }
  if (!fConfig->GetMinimalBookingME() && fPartColl && fPartColl->GetQAList()) {
    fResultQAList = fPartColl->GetQAList();
  } else {
    fResultQAList = new TList();
    fResultQAList->SetName("ResultsQA");
    fResultQAList->SetOwner(true);
  }

  //ML model
  if(fApplyML) {
    if(!fDependOnMLSelector) {
      switch(fDecChannel) {
        case kDplustoKpipi:
            fMLResponse = new AliHFMLResponseDplustoKpipi("DplustoKpipiMLResponse", "DplustoKpipiMLResponse", fConfigPath.Data());
            fMLResponse->MLResponseInit();
            break;
        case kDstartoKpipi:
            fMLResponse = new AliHFMLResponseDstartoD0pi("DstartoD0piMLResponse", "DstartoD0piMLResponse", fConfigPath.Data());
            fMLResponse->MLResponseInit();
            break;
      }
    }
    else {
      std::string configLocalPath = AliMLModelHandler::ImportFile(fConfigPath.Data());
      YAML::Node nodeList;
      try {
        nodeList = YAML::LoadFile(configLocalPath);
      } catch (std::exception &e) {
        AliFatal(Form("Yaml-ccp error: %s! Exit", e.what()));
      }
      fPtLimsML = nodeList["BINS"].as<vector<float> >();

      for (const auto &model : nodeList["MODELS"]) {
        fMLScoreCuts.push_back(model["cut"].as<std::vector<double> >());
        fMLOptScoreCuts.push_back(model["cut_opt"].as<std::vector<std::string> >());
      }
    }
  }

  PostData(1, fQA);
  PostData(2, fEvtHistList);
  PostData(3, fTrackCutHistList);
  PostData(4, fAntiTrackCutHistList);
  PostData(5, fDChargedHistList);
  PostData(6, fResultList);
  PostData(7, fResultQAList);
  if (fIsMC) {
    PostData(9, fTrackCutHistMCList);
    PostData(10, fAntiTrackCutHistMCList);
  }
}

//________________________________________________________________________
int AliAnalysisTaskCharmingFemto::IsCandidateSelected(AliAODRecoDecayHF *&dMeson, AliAODRecoDecayHF *&dMesonWithVtx, int absPdgMom, bool &unsetVtx, bool &recVtx, AliAODVertex *&origOwnVtx, std::vector<double> scores) {

  if(!dMeson || !dMesonWithVtx) {
    return 0;
  }

  bool isSelBit = true;
  switch(fDecChannel) {
    case kDplustoKpipi:
      isSelBit = dMeson->HasSelectionBit(AliRDHFCuts::kDplusCuts);
      if(!isSelBit) {
        return 0;
      }
      break;
    case kDstartoKpipi:
      isSelBit = dMeson->HasSelectionBit(AliRDHFCuts::kDstarCuts);
      if(!isSelBit) {
        return 0;
      }
      break;
  }

  unsetVtx = false;
  if (!dMesonWithVtx->GetOwnPrimaryVtx())
  {
    dMesonWithVtx->SetOwnPrimaryVtx(dynamic_cast<AliAODVertex *>(fInputEvent->GetPrimaryVertex()));
    unsetVtx = true;
    // NOTE: the own primary vertex should be unset, otherwise there is a memory leak
    // Pay attention if you use continue inside this loop!!!
  }

  double ptD = dMeson->Pt();
  double yD = dMeson->Y(absPdgMom);
  int ptbin = fRDHFCuts->PtBin(ptD);
  if(ptbin < 0) {
    if (unsetVtx) {
      dMesonWithVtx->UnsetOwnPrimaryVtx();
    }
    return 0;
  }

  bool isFidAcc = fRDHFCuts->IsInFiducialAcceptance(ptD, yD);
  if(!isFidAcc) {
    if (unsetVtx) {
      dMesonWithVtx->UnsetOwnPrimaryVtx();
    }
    return 0;
  }

  int isSelected = fRDHFCuts->IsSelected(dMeson, AliRDHFCuts::kAll, fInputEvent);
  if(!isSelected) {
    if (unsetVtx) {
      dMesonWithVtx->UnsetOwnPrimaryVtx();
    }
    return 0;
  }

  // ML application
  if(fApplyML) {
    if(!fDependOnMLSelector) { //direct application
      AliAODPidHF* pidHF = fRDHFCuts->GetPidHF();
      bool isMLsel = true;
      std::vector<double> modelPred{};
      switch(fDecChannel) {
        case kDplustoKpipi:
          isMLsel = fMLResponse->IsSelectedMultiClass(modelPred, dMeson, fInputEvent->GetMagneticField(), pidHF);
          if(!isMLsel) {
            isSelected = 0;
          }
          break;
        case kDstartoKpipi:
          isMLsel = fMLResponse->IsSelectedMultiClass(modelPred, dMeson, fInputEvent->GetMagneticField(), pidHF);
          if(!isMLsel) {
            isSelected = 0;
          }
          break;
      }
    }
    else { // read result from common task
      std::vector<float>::iterator low = std::lower_bound(fPtLimsML.begin(), fPtLimsML.end(), ptD);
      int bin = low - fPtLimsML.begin() - 1;
      if(bin < 0)
        bin = 0;
      else if(bin > fPtLimsML.size()-2)
        bin = fPtLimsML.size()-2;
      for(size_t iScore = 0; iScore < scores.size(); iScore++) {
        if((fMLOptScoreCuts[bin][iScore] == "upper" && scores[iScore] > fMLScoreCuts[bin][iScore]) || (fMLOptScoreCuts[bin][iScore] == "lower" && scores[iScore] < fMLScoreCuts[bin][iScore])){
          isSelected = 0;
          break;
        }
      }
    }
  }

  recVtx = false;
  origOwnVtx = nullptr;

  if (fRDHFCuts->GetIsPrimaryWithoutDaughters())
  {
    if (dMesonWithVtx->GetOwnPrimaryVtx()) {
      origOwnVtx = new AliAODVertex(*dMesonWithVtx->GetOwnPrimaryVtx());
    }
    if (fRDHFCuts->RecalcOwnPrimaryVtx(dMesonWithVtx, fInputEvent)) {
      recVtx = true;
    }
    else {
      fRDHFCuts->CleanOwnPrimaryVtx(dMesonWithVtx, fInputEvent, origOwnVtx);
    }
  }

  return isSelected;
}

//____________________________________________________________________________________________________
bool AliAnalysisTaskCharmingFemto::MassSelection(const double mass,
                                                 const double pt,
                                                 const int pdg) {
  // simple parametrisation from D+ in 5.02 TeV
  double massMean = TDatabasePDG::Instance()->GetParticle(pdg)->Mass() + 0.0025;  // mass shift observed in all Run2 data samples for all
                                                                                  // D-meson species
  double massWidth = 0.;
  switch (fDecChannel) {
    case kDplustoKpipi:
      if (fSystem == kpp5TeV) {
        massWidth = 0.0057 + pt * 0.00066;
      } else if (fSystem == kpp13TeV) {
        massWidth = 0.006758 + pt * 0.0005124;
      }
      break;
    case kDstartoKpipi:
      Double_t mDstarPDG = TDatabasePDG::Instance()->GetParticle(413)->Mass();
      Double_t mD0PDG = TDatabasePDG::Instance()->GetParticle(421)->Mass();
      massMean = mDstarPDG-mD0PDG; // no extra mass shift because it is deltamass
      if (fSystem == kpp5TeV) {
        massWidth = 0.00105236 - pt * 0.000255556 + pt * pt * 3.2264e-05;
        if(pt > 4 && pt < 5) massWidth = 0.000606852 - 0.000015123 * pt;
        else if(pt >= 5) massWidth = 0.000476887 + pt * 1.087e-05;
      } else if (fSystem == kpp13TeV) {
        massWidth = 0.00124673 - pt * 0.000340426 + pt * pt * 4.40729e-05;
        if(pt > 4 && pt < 5) massWidth = 0.00104329 - 0.000113275 * pt;
        else if(pt >= 5) massWidth = 0.000519861 - 8.58874e-06 * pt;
      }
      break;
  }

  // select D mesons mass window
  if (fMassSelectionType == kSignal) {
    fLowerMassSelection = massMean - fNSigmaMass * massWidth;
    fUpperMassSelection = massMean + fNSigmaMass * massWidth;
  } else if ( fMassSelectionType == kSidebandLeft) {
    fLowerMassSelection = massMean - fNSigmaOffsetSideband * massWidth - fSidebandWidth;
    fUpperMassSelection = massMean - fNSigmaOffsetSideband * massWidth;
  } else if ( fMassSelectionType == kSidebandRight) {
    fLowerMassSelection = massMean + fNSigmaOffsetSideband * massWidth;
    fUpperMassSelection = massMean + fNSigmaOffsetSideband * massWidth + fSidebandWidth;

    if(fDecChannel == kDplustoKpipi){
      // additional removal of D*
      if ( mass > fLowerDstarRemoval && mass < fUpperDstarRemoval) {
        return false;
      }
    }
  }


  if (mass > fLowerMassSelection && mass < fUpperMassSelection) {
    return true;
  }

  return false;
}
