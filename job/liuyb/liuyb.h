//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 10 14:52:51 2014 by ROOT version 5.26/00e
// from TTree Singles/SinglesTree
// found on file: /publicfs/dyb/data/userdata/liuyb/LAF/SinglesAlg/run/P13A/EH1/data/P13A/singles_36813.root
//////////////////////////////////////////////////////////

#ifndef liuyb_h
#define liuyb_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include  <TH1F.h>
#include <TSelector.h>
#include  <iostream>
using namespace std;

class liuyb : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   TH1F* slice[5];
   TFile* f;
   Float_t timeInSlice[5][1000];
   Float_t minTime[5];
   Float_t energyRange[6];
   Int_t           Det;
   Float_t         Time2Pre;
   Float_t         Time2Post;
   Float_t         PreEnergy;
   Float_t         PostEnergy;
   Int_t           TrigSec;
   Int_t           TrigNanoSec;
   Float_t         Enrec;
   Float_t         Eprec;
   Float_t         AdSimpleX;
   Float_t         AdSimpleY;
   Float_t         AdSimpleZ;
   Float_t         X;
   Float_t         Y;
   Float_t         Z;
   Float_t         XPre;
   Float_t         YPre;
   Float_t         ZPre;
   Float_t         XPost;
   Float_t         YPost;
   Float_t         ZPost;
   Int_t           MuonMultiplicity;
   Float_t         MuonEnergy[1000];   //[MuonMultiplicity]
   Float_t         SingleTime2Muon[1000];   //[MuonMultiplicity]
   Int_t           NeuMultiplicityHGd[1000];   //[MuonMultiplicity]
   Float_t         Quadrant;
   Float_t         MaxQ;

   // List of branches
   TBranch        *b_Det;   //!
   TBranch        *b_Time2Pre;   //!
   TBranch        *b_Time2Post;   //!
   TBranch        *b_PreEnergy;   //!
   TBranch        *b_PostEnergy;   //!
   TBranch        *b_TrigSec;   //!
   TBranch        *b_TrigNanoSec;   //!
   TBranch        *b_Enrec;   //!
   TBranch        *b_Eprec;   //!
   TBranch        *b_AdSimpleX;   //!
   TBranch        *b_AdSimpleY;   //!
   TBranch        *b_AdSimpleZ;   //!
   TBranch        *b_X;   //!
   TBranch        *b_Y;   //!
   TBranch        *b_Z;   //!
   TBranch        *b_XPre;   //!
   TBranch        *b_YPre;   //!
   TBranch        *b_ZPre;   //!
   TBranch        *b_XPost;   //!
   TBranch        *b_YPost;   //!
   TBranch        *b_ZPost;   //!
   TBranch        *b_MuonMultiplicity;   //!
   TBranch        *b_MuonEnergy;   //!
   TBranch        *b_SingleTime2Muon;   //!
   TBranch        *b_NeuMultiplicityHGd;   //!
   TBranch        *b_Quadrant;   //!
   TBranch        *b_MaxQ;   //!

   liuyb(TTree * /*tree*/ =0) { }
   virtual ~liuyb() { }
   virtual Int_t   Version() const { return 2; }
   virtual void    Begin(TTree *tree);
   virtual void    SlaveBegin(TTree *tree);
   virtual void    Init(TTree *tree);
   virtual Bool_t  Notify();
   virtual Bool_t  Process(Long64_t entry);
   virtual Int_t   GetEntry(Long64_t entry, Int_t getall = 0) { return fChain ? fChain->GetTree()->GetEntry(entry, getall) : 0; }
   virtual void    SetOption(const char *option) { fOption = option; }
   virtual void    SetObject(TObject *obj) { fObject = obj; }
   virtual void    SetInputList(TList *input) { fInput = input; }
   virtual TList  *GetOutputList() const { return fOutput; }
   virtual void    SlaveTerminate();
   virtual void    Terminate();

   ClassDef(liuyb,0);
};

#endif

#ifdef liuyb_cxx
void liuyb::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Det", &Det, &b_Det);
   fChain->SetBranchAddress("Time2Pre", &Time2Pre, &b_Time2Pre);
   fChain->SetBranchAddress("Time2Post", &Time2Post, &b_Time2Post);
   fChain->SetBranchAddress("PreEnergy", &PreEnergy, &b_PreEnergy);
   fChain->SetBranchAddress("PostEnergy", &PostEnergy, &b_PostEnergy);
   fChain->SetBranchAddress("TrigSec", &TrigSec, &b_TrigSec);
   fChain->SetBranchAddress("TrigNanoSec", &TrigNanoSec, &b_TrigNanoSec);
   fChain->SetBranchAddress("Enrec", &Enrec, &b_Enrec);
   fChain->SetBranchAddress("Eprec", &Eprec, &b_Eprec);
   fChain->SetBranchAddress("AdSimpleX", &AdSimpleX, &b_AdSimpleX);
   fChain->SetBranchAddress("AdSimpleY", &AdSimpleY, &b_AdSimpleY);
   fChain->SetBranchAddress("AdSimpleZ", &AdSimpleZ, &b_AdSimpleZ);
   fChain->SetBranchAddress("X", &X, &b_X);
   fChain->SetBranchAddress("Y", &Y, &b_Y);
   fChain->SetBranchAddress("Z", &Z, &b_Z);
   fChain->SetBranchAddress("XPre", &XPre, &b_XPre);
   fChain->SetBranchAddress("YPre", &YPre, &b_YPre);
   fChain->SetBranchAddress("ZPre", &ZPre, &b_ZPre);
   fChain->SetBranchAddress("XPost", &XPost, &b_XPost);
   fChain->SetBranchAddress("YPost", &YPost, &b_YPost);
   fChain->SetBranchAddress("ZPost", &ZPost, &b_ZPost);
   fChain->SetBranchAddress("MuonMultiplicity", &MuonMultiplicity, &b_MuonMultiplicity);
   fChain->SetBranchAddress("MuonEnergy", MuonEnergy, &b_MuonEnergy);
   fChain->SetBranchAddress("SingleTime2Muon", SingleTime2Muon, &b_SingleTime2Muon);
   fChain->SetBranchAddress("NeuMultiplicityHGd", NeuMultiplicityHGd, &b_NeuMultiplicityHGd);
   fChain->SetBranchAddress("Quadrant", &Quadrant, &b_Quadrant);
   fChain->SetBranchAddress("MaxQ", &MaxQ, &b_MaxQ);
}

Bool_t liuyb::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef liuyb_cxx
