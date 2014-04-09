#define liuyb_cxx
// The class definition in liuyb.h has been generated automatically
// by the ROOT utility TTree::MakeSelector(). This class is derived
// from the ROOT class TSelector. For more information on the TSelector
// framework see $ROOTSYS/README/README.SELECTOR or the ROOT User Manual.

// The following methods are defined in this file:
//    Begin():        called every time a loop on the tree starts,
//                    a convenient place to create your histograms.
//    SlaveBegin():   called after Begin(), when on PROOF called only on the
//                    slave servers.
//    Process():      called for each event, in this function you decide what
//                    to read and fill your histograms.
//    SlaveTerminate: called at the end of the loop on the tree, when on PROOF
//                    called only on the slave servers.
//    Terminate():    called at the end of the loop on the tree,
//                    a convenient place to draw/fit your histograms.
//
// To use this file, try the following session on your Tree T:
//
// Root > T->Process("liuyb.C")
// Root > T->Process("liuyb.C","some options")
// Root > T->Process("liuyb.C+")
//

#include "liuyb.h"
#include <TH2.h>
#include <TStyle.h>

#include  <TString.h>

void liuyb::Begin(TTree * /*tree*/)
{
   // The Begin() function is called at the start of the query.
   // When running with PROOF Begin() is only called on the client.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();
   energyRange[0]=20.;
   energyRange[1]=60.;
   energyRange[2]=500.;
   energyRange[3]=1500.;
   energyRange[4]=2500.;
   energyRange[5]=10000.;
   

   f=new TFile("5slice.root","RECREATE");
   for( int i=0 ; i<5 ; i++ )
   {
       TString hname;
       hname="slice";
       hname+=i+1;
       slice[i]=new TH1F(hname,"slice histo",4000,0.001,20.001);
   }
   
}

void liuyb::SlaveBegin(TTree * /*tree*/)
{
   // The SlaveBegin() function is called after the Begin() function.
   // When running with PROOF SlaveBegin() is called on each slave server.
   // The tree argument is deprecated (on PROOF 0 is passed).

   TString option = GetOption();

}

Bool_t liuyb::Process(Long64_t entry)
{
   // The Process() function is called for each entry in the tree (or possibly
   // keyed object in the case of PROOF) to be processed. The entry argument
   // specifies which entry in the currently loaded tree is to be processed.
   // It can be passed to either liuyb::GetEntry() or TBranch::GetEntry()
   // to read either all or the required parts of the data. When processing
   // keyed objects with PROOF, the object is already loaded and is available
   // via the fObject pointer.
   //
   // This function should contain the "body" of the analysis. It can contain
   // simple or elaborate selection criteria, run algorithms on the data
   // of the event and typically fill histograms.
   //
   // The processing can be stopped by calling Abort().
   //
   // Use fStatus to set the return value of TTree::Process().
   //
   // The return value is currently not used.
    GetEntry(entry);
    if( entry%1000==0 )
    {
        std::cout<<entry<<endl;
    }
    if( Enrec>12. || Enrec<6 )
    {
        return true;
    }
   for( int i=0 ; i<1000 ; i++ )
   {
      for( int j=0 ; j<5 ; j++ )
      {
          timeInSlice[j][i]=0.;
      }
      
   }
   for( int i=0 ; i<5 ; i++ )
   {
       minTime[i]=1000.;
   }
   
    for( int i=0 ; i<MuonMultiplicity ; i++ )
    {
        //std::cout<<"now is   : "<<i<<endl;
        for( int j=0 ; j<5 ; j++ )
        {
            if( MuonEnergy[i]>energyRange[j] &&MuonEnergy[i]<energyRange[j+1] )
            {
                if( SingleTime2Muon[i]!=0 )
                {
                    minTime[j]=minTime[j]<SingleTime2Muon[i]?minTime[j]:SingleTime2Muon[i];
                }
                
                //timeInSlice[j][i]=SingleTime2Muon[i];
                break;
            }
        }
    }
    for( int i=0 ; i<5 ; i++ )
    {
        //std::cout<<"timeInSlice "<<i+1<<": "<< endl;
        //std::cout<<"minTime  : "<<minTime[i]<<endl;
        if( minTime[i]!=1000. )
        {
            slice[i]->Fill(minTime[i]);
        }
        //for( int j=0 ; j<1000 ; j++ )
        //{
            //if( timeInSlice[i][j]!=0. )
            //{
                
                //std::cout<<" "<<timeInSlice[i][j]<<" ";
            //}else
            //{
                //std::cout<<timeInSlice[i][j];
            //}
        //}
        //std::cout<<" "<<endl;
        
    }
    //std::cout<<" "<<endl;
    

   return kTRUE;
}

void liuyb::SlaveTerminate()
{
   // The SlaveTerminate() function is called after all entries or objects
   // have been processed. When running with PROOF SlaveTerminate() is called
   // on each slave server.

}

void liuyb::Terminate()
{
   // The Terminate() function is the last function to be called during
   // a query. It always runs on the client, it can be used to present
   // the results graphically or save the results to file.

    for( int i=0 ; i<5 ; i++ )
    {
        slice[i]->Write();
    }
    
}
