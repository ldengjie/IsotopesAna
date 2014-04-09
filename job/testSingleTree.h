//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Mar 25 15:06:45 2014 by ROOT version 5.26/00e
// from TTree SingleTree/SingleTree
// found on file: /afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P14A/EH1/run40035_IsotopesAna.root
//////////////////////////////////////////////////////////

#ifndef testSingleTree_h
#define testSingleTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TSelector.h>

class testSingleTree : public TSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain

   // Declaration of leaf types
   Float_t         energy;
   Float_t         x;
   Float_t         y;
   Float_t         z;
   Int_t           det;
   Float_t         t2lastshowermuon;
   Float_t         T2lastMuon[3564];

   // List of branches
   TBranch        *b_energy;   //!
   TBranch        *b_x;   //!
   TBranch        *b_y;   //!
   TBranch        *b_z;   //!
   TBranch        *b_det;   //!
   TBranch        *b_t2lastshowermuon;   //!
   TBranch        *b_T2lastMuon;   //!

   testSingleTree(TTree * /*tree*/ =0) { }
   virtual ~testSingleTree() { }
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

   ClassDef(testSingleTree,0);
};

#endif

#ifdef testSingleTree_cxx
void testSingleTree::Init(TTree *tree)
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
   fChain->SetBranchAddress("T2lastMuon", T2lastMuon, &b_T2lastMuon);
}

Bool_t testSingleTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

#endif // #ifdef testSingleTree_cxx
