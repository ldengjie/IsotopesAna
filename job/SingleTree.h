//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Dec 10 04:38:04 2013 by ROOT version 5.26/00e
// from TTree SingleTree/SingleTree
// found on file: /afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12E/EH1/run21868_IsotopesAna.root
//////////////////////////////////////////////////////////

#ifndef SingleTree_h
#define SingleTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>
#include <TH1F.h>
#include <TH2F.h>
#include  <TCanvas.h>
#include  <TLegend.h>

class SingleTree : public TSelector {
    public :
        TTree          *fChain;   //!pointer to the analyzed TTree or TChain

        bool genIsoSpec,genAmCNeu,genTimeFitDis;
        TString histname;
        char nameChar[100];
        double LowEdge;
        double HighEdge;
        double LowEdge4e;
        double HighEdge4e;
        double signalWinLow;
        double signalWinHigh;
        double offWinLow;
        double offWinHigh;
        string IsoMode;
        TFile* file;
        TString dataVer;
        int ADNum;
        int site;
        TH2F* singleSpecVsTime[2][6];
        TH1F* signalWin[2][6];
        TH1F* offWin[2][6];
        int offTheoNum[2][6];
        int offRealNum[2][6];
        //double offCoe[2][6];
        bool isRealOff;
        TH2F* signalWinXY[2][6];
        TH2F* offWinXY[2][6];
        TH2F* signalWinRZ[2][6];
        TH2F* offWinRZ[2][6];
        TH1F* isoSpec[2][6];
        string ifRed[2];
        TH1F* singleUpper[4];
        TH1F* singleLower[4];
        TH1F* AmCSpec[4];
        TH1F* time2Allmuon[2];
        TH1F* time2lastshowermuonNoRed[5];
        TH1F* time2lastshowermuon[5];
        TTree* time2lastmuon[7];
        TTree* time2lastmuonNoRed[6];
        Float_t minT2allmuon[2];
        //double slicetime[6];
        //double slicetimeNoRed[6];
        // Declaration of leaf types
        Float_t         energy;
        Float_t         x;
        Float_t         y;
        Float_t         z;
        Int_t           det;
        Float_t         t2lastshowermuon;
        Float_t         T2lastMuon[16];

        // List of branches
        TBranch        *b_energy;   //!
        TBranch        *b_x;   //!
        TBranch        *b_y;   //!
        TBranch        *b_z;   //!
        TBranch        *b_det;   //!
        TBranch        *b_t2lastshowermuon;   //!
        TBranch        *b_T2lastMuon;   //!

        SingleTree(TTree * /*tree*/ =0) { }
        virtual ~SingleTree() { }
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

        ClassDef(SingleTree,0);
};

#endif

#ifdef SingleTree_cxx
void SingleTree::Init(TTree *tree)
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

    fChain->SetBranchAddress("energy", &energy, &b_energy);
    fChain->SetBranchAddress("x", &x, &b_x);
    fChain->SetBranchAddress("y", &y, &b_y);
    fChain->SetBranchAddress("z", &z, &b_z);
    fChain->SetBranchAddress("det", &det, &b_det);
    fChain->SetBranchAddress("t2lastshowermuon", &t2lastshowermuon, &b_t2lastshowermuon);
    fChain->SetBranchAddress("T2lastMuon", &T2lastMuon, &b_T2lastMuon);
}

Bool_t SingleTree::Notify()
{
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

#endif // #ifdef SingleTree_cxx
