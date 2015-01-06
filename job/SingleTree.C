#define SingleTree_cxx

#include "SingleTree.h"
#include <TH2.h>
#include <TStyle.h>
#include  <THStack.h>
#include  <iostream>

void SingleTree::Begin(TTree * /*tree*/)
{
    TString option = GetOption();
    TString IsoModeTmp=option(7,3);
    TString evtNumStr=option(10,(option.Sizeof()-10));
    totalEntries=evtNumStr.Atoi();
    cout<<"totalEntries  : "<<totalEntries<<endl;
    genIsoSpec=1;
    genAmCNeu=0;
    genTimeFitDis=1;
    //IsoMode="C9";//Li8,C99,B12
    IsoMode=IsoModeTmp;//Li8,C99,B12
    ifRed[1]="NoRed";
    ifRed[0]="";
    binNum=80;
    for( int i=0 ; i<52 ; i++ )
    {
        T2lastMuon[i]=0.;
    }

    if( IsoMode=="B12" )
    {
        LowEdge=5.5;
        //LowEdge=4.0;
        HighEdge=20.0;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        //signalWinLow=0.002;
        //signalWinHigh=0.06;
        //offWinLow=0.502;
        //offWinHigh=0.560;
        signalWinLow=0.002;
        signalWinHigh=0.100;
        offWinLow=-0.100;
        offWinHigh=-0.002;
    }else if( IsoMode=="Li8" )
    {
        //LowEdge=5.5;
        LowEdge=4.0;
        HighEdge=20.0;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.6;
        signalWinHigh=4.0;
        offWinLow=10.6;
        offWinHigh=14.0;
    }else if( IsoMode=="N12" )
    {
        LowEdge=14.0;//for N12 time fit,that for Li8 has been done while B12.
        HighEdge=20.0;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        //signalWinLow=0.002;
        //signalWinHigh=0.06;
        //offWinLow=0.502;
        //offWinHigh=0.560;
        signalWinLow=0.002;
        signalWinHigh=0.082;
        offWinLow=0.502;
        offWinHigh=0.583;
    } else if( IsoMode=="C99" )
    {
        LowEdge=12.0;
        HighEdge=20.0;
        LowEdge4e=LowEdge;
        HighEdge4e=HighEdge;
        signalWinLow=0.2;
        signalWinHigh=0.6;
        offWinLow=10.2;
        offWinHigh=10.6;
    }


    TString DataVerTmp=option(3,4);
    dataVer=DataVerTmp;
    option=option(0,3);
    std::cout<<"dataVer  : "<<dataVer<<endl;
    int daqHistNum;
    if( dataVer=="P12E"||dataVer=="P12C" )
    {
        daqHistNum=4;
    } else
    {
        daqHistNum=5;
    }
    std::cout<<"option  : "<<option<<endl;
    ADNum=daqHistNum-1;
    site=2;
    if( option=="EH1")
    {
        site=0;
        ADNum=2;
    }
    if( option=="EH2" )
    {
        site=1;
        ADNum=daqHistNum-3;
    }
    option+="iso_";
    option+=dataVer;
    option+=".root";
    option=dataVer+"/"+option;
    file = new TFile(option,"update");

    //for( int k=0 ; k<3 ; k++ )
    //{
    //presentMuonTriggetime[k]=0.;
    //for( int i=0 ; i<6 ; i++ )
    //{
    //nextMuonTriggetime[k][i]=0.;
    //preMuonTriggetime[k][i]=0.;
    //}
    //
    //}
            
    if( genTimeFitDis )
    {
        //histogram - binned data for MML or Chi2
        for( int j=0 ; j<3 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {
                histname=Form("I%dtime2lastshowermuon%i_%0.1f_%0.1f",j+1,i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                Itime2lastshowermuon[j][i]=new TH1F(histname,"Itime2lastshowermuon",99999,0.001,100);
            }
                histname=Form("I%dtime2lastshowermuon123_%0.1f_%0.1f",j+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                Itime2lastshowermuon[j][6]=new TH1F(histname,"Itime2lastshowermuon",99999,0.001,100);

        }
        for( int i=0 ; i<5 ; i++ )
        {
            histname=Form("time2lastshowermuon%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            time2lastshowermuon[i]=new TH1F(histname,"time2lastshowermuon",99999,0.001,100);
            histname=Form("time2lastshowermuonNoRed%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            time2lastshowermuonNoRed[i]=new TH1F(histname,"time2lastshowermuon",99999,0.001,100);
        }

        histname=Form("time2Allmuon_%0.1f_%0.1f",LowEdge,HighEdge);
        histname2=histname+";*";
        gDirectory->Delete(histname2);
        time2Allmuon[0]=new TH1F(histname,"time2Allmuon",99999,0.001,100);
        histname=Form("time2AllmuonNoRed_%0.1f_%0.1f",LowEdge,HighEdge);
        histname2=histname+";*";
        gDirectory->Delete(histname2);
        time2Allmuon[1]=new TH1F(histname,"time2Allmuon",99999,0.001,100);

        //tree - unbinned data for MML
        for( int i=0 ; i<5 ; i++ )
        {
            histname=Form("slice%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            time2lastmuon[i]=new TTree(histname,"time2lastmuon");
            histname=Form("sliceNoRed%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            time2lastmuonNoRed[i]=new TTree(histname,"time2lastmuon NoRed");

            time2lastmuon[i]->Branch("xt",&T2lastMuon[4+i],"xt/D");
            time2lastmuonNoRed[i]->Branch("xt",&T2lastMuon[10+i],"xt/D");
        }
        histname=Form("slice6_%0.1f_%0.1f",LowEdge,HighEdge);
        histname2=histname+";*";
        gDirectory->Delete(histname2);
        time2lastmuon[5]=new TTree(histname,"time2lastmuon");
        time2lastmuon[5]->Branch("xt",&minT2allmuon[0],"xt/D");
        histname=Form("sliceNoRed6_%0.1f_%0.1f",LowEdge,HighEdge);
        histname2=histname+";*";
        gDirectory->Delete(histname2);
        time2lastmuonNoRed[5]=new TTree(histname,"time2lastmuon NoRed");
        time2lastmuonNoRed[5]->Branch("xt",&minT2allmuon[1],"xt/D");
    }
    if( genAmCNeu )
    {
        for( int i=0 ; i<4 ; i++ )
        {
            histname=Form("singleUpperAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            singleUpper[i]=new TH1F(histname,"single upper",400,0,20);
            histname=Form("singleLowerAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            singleLower[i]=new TH1F(histname,"single lower",400,0,20);
            histname=Form("AmCSpecAD%i_%0.1f_%0.1f",i+1,LowEdge,HighEdge);
            histname2=histname+";*";
            gDirectory->Delete(histname2);
            AmCSpec[i]=new TH1F(histname,"AmC spectrum",400,0,20);
        }

    }

    if( genIsoSpec )
    {
        //energy spectrum after muon that meet 100 200 300 ms time isolation cut
        for( int j=0 ; j<3 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {

                histname=Form("%sI%dsignalEnergySlice%i_%0.1f_%0.1f",IsoMode.c_str(),j+1,i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                IsignalWin[j][i]=new TH1F(histname,histname,binNum,0,20);
                histname=Form("%sI%doffEnergySlice%i_%0.1f_%0.1f",IsoMode.c_str(),j+1,i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                IoffWin[j][i]=new TH1F(histname,histname,binNum,0,20);
                histname=Form("%sI%dSpecSlice%i_%0.1f_%0.1f",IsoMode.c_str(),j+1,i+1,LowEdge4e,HighEdge4e);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                IisoSpec[j][i]=new TH1F(histname,histname,binNum,0,20);
            }
                histname=Form("%sI%dsignalEnergySlice123_%0.1f_%0.1f",IsoMode.c_str(),j+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                IsignalWin[j][6]=new TH1F(histname,histname,binNum,0,20);
                histname=Form("%sI%doffEnergySlice123_%0.1f_%0.1f",IsoMode.c_str(),j+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                IoffWin[j][6]=new TH1F(histname,histname,binNum,0,20);
                histname=Form("%sI%dSpecSlice123_%0.1f_%0.1f",IsoMode.c_str(),j+1,LowEdge4e,HighEdge4e);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                IisoSpec[j][6]=new TH1F(histname,histname,binNum,0,20);

        }

        //energy spectrum / xy / rz /specVsTime  after muon w/o reduction cut ( there are neutrons after it).
        for( int j=0 ; j<2 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {
                offTheoNum[j][i]=1;
                offRealNum[j][i]=1;
                histname=Form("%ssignalEnergy%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                signalWin[j][i]=new TH1F(histname,histname,binNum,0,20);
                histname=Form("%soffEnergy%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                offWin[j][i]=new TH1F(histname,histname,binNum,0,20);
                histname=Form("%ssignalXY%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                std::cout<<"histname  : "<<histname<<endl;
                signalWinXY[j][i]=new TH2F(histname,histname,600,-3000.,3000.,600,-3000.,3000.);
                histname=Form("%soffXY%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                offWinXY[j][i]=new TH2F(histname,histname,600,-3000,3000,600,-3000,3000);
                histname=Form("%ssignalRZ%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                signalWinRZ[j][i]=new TH2F(histname,histname,300,0,3000,600,-3000,3000);
                histname=Form("%soffRZ%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                offWinRZ[j][i]=new TH2F(histname,histname,300,0,3000,600,-3000,3000);
                histname=Form("%ssingleSpecVsTime%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge,HighEdge);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                singleSpecVsTime[j][i]=new TH2F(histname,histname,100000,0,100,400,0,20);
            }
        }
    }
    std::cout<<"finished Begin() "<<endl;
}

void SingleTree::SlaveBegin(TTree * /*tree*/)
{

    TString option = GetOption();

}

Bool_t SingleTree::Process(Long64_t entry)
{
    GetEntry(entry);
    //if( entry%10000==0 )
    //{
    //std::cout<<entry<<endl;
    //}
    
    //cut all events from Z>=900;cut flahser that exists in P12E P13A  from this zone;
    if(!(z<900&&!(x>-1700&&x<-1200&&y>-1500&&y<-900&&z>-900&&x<-500)) )
    {
        return true;
    }

    //isotopes spectrum
    if( genIsoSpec )
    {
        if( energy>=LowEdge4e&&energy<=HighEdge4e )
        {
            //for muon reduction cut
            for( int i=0 ; i<2 ; i++ )
            {
                minT2allmuonI[i]=1000.;
                for( int j=0 ; j<5 ; j++ )
                {
                    singleSpecVsTime[i][j]->Fill(T2lastMuon[j+16+6*i],energy);
                    minT2allmuonI[i]=minT2allmuonI[i]<T2lastMuon[j+16+6*i]?minT2allmuonI[i]:T2lastMuon[j+16+6*i];
                    if( T2lastMuon[j+16+6*i]>=signalWinLow &&T2lastMuon[j+16+6*i]<=signalWinHigh )
                    {
                        signalWinXY[i][j]->Fill(x,y);
                        signalWinRZ[i][j]->Fill(sqrt(x*x+y*y),z);
                        signalWin[i][j]->Fill(energy);
                    }
                    if( T2lastMuon[j+16+6*i]>=offWinLow &&T2lastMuon[j+16+6*i]<=offWinHigh )
                    {
                        offWinXY[i][j]->Fill(x,y);
                        offWinRZ[i][j]->Fill(sqrt(x*x+y*y),z);
                        offTheoNum[i][j]++;
                        isRealOff=1; 
                        for( int a=0 ; a<5 ; a++ )
                        {
                            if( T2lastMuon[a+16+6*i]>=signalWinLow&&T2lastMuon[a+16+6*i]<=signalWinHigh )
                            {
                                isRealOff=0;    
                                break;
                            }
                        }
                        if( isRealOff )
                        {
                            offWin[i][j]->Fill(energy);
                            offRealNum[i][j]++;
                        } 
                    }
                }
                singleSpecVsTime[i][5]->Fill(minT2allmuonI[i],energy);
                if( minT2allmuonI[i]>=signalWinLow &&minT2allmuonI[i]<=signalWinHigh )
                {
                    signalWinXY[i][5]->Fill(x,y);
                    signalWinRZ[i][5]->Fill(sqrt(x*x+y*y),z);
                    signalWin[i][5]->Fill(energy);
                }
                if( minT2allmuonI[i]>=offWinLow &&minT2allmuonI[i]<=offWinHigh )
                {
                    offWinXY[i][5]->Fill(x,y);
                    offWinRZ[i][5]->Fill(sqrt(x*x+y*y),z);
                    offWin[i][5]->Fill(energy);
                }
            }

            //for time isolation cut
            /*
            //calculate time to next muon
            presentTriggerTime=triggerTime;
            for(int k=0;k<3;k++) presentMuonTriggetime[k]=T2lastMuon[34+k];

            //find out next muon
            for( int k=0 ; k<3 ; k++ )
            {
                bool findOut=0;
                bool needUpdate6=0;
                //for( int i=0 ; i<5 ; i++ )
                //{
                //cout<<" "<<endl;
                //cout<<"   "<<presentTriggerTime<<" - "<<nextMuonTriggetime[k][i]<<" = "<<presentTriggerTime-nextMuonTriggetime[k][i]<<endl;
                //if( presentTriggerTime>nextMuonTriggetime[k][i] )
                if((T2lastMuon[34+k]==nextMuonTriggetime[k][5]) || (nextMuonTriggetime[k][5]==0.))
                {
                    needUpdate6=1;
                    int i=0;
                    for(  ; i<5 ; i++ )
                    {
                        cout<<"k-i  : "<<k<<"-"<<i<<endl;
                        cout<<presentTriggerTime-T2lastMuon[34+k]<<"  =? "<<T2lastMuon[i+10+(k+1)*6]<<endl;
                        cout<<"   "<<abs((presentTriggerTime-T2lastMuon[34+k])-T2lastMuon[i+10+(k+1)*6])<<endl;
                        if( abs((presentTriggerTime-T2lastMuon[34+k])-T2lastMuon[i+10+(k+1)*6])<1.e-6 )
                        {
                            //if( i==i )
                            //{
                            preMuonTriggetime[k][i]=T2lastMuon[34+k];
                            preMuonTriggetime[k][5]=T2lastMuon[34+k];
                            break;
                            //}
                        }
                    }
                    for( int e=entry+1 ; e<totalEntries ; e++ )
                    {
                        GetEntry(e);
                        if( T2lastMuon[34+k]-presentTriggerTime>100.e-3 ) break;
                        if( T2lastMuon[34+k]>presentMuonTriggetime[k])
                        {
                            //for( int t=0 ; t<5 ; t++ )
                            //{
                            if( abs((triggerTime-T2lastMuon[34+k])-T2lastMuon[i+10+(k+1)*6])<1.e-6 )
                            {
                                //if( t==i )
                                //{
                                nextMuonTriggetime[k][i]=T2lastMuon[34+k];
                                findOut=1;
                                break;
                                //}
                            }
                            //}

                        }
                        //if( findOut ) break;
                    }
                    GetEntry(entry);
                }
                //}
                if( needUpdate6)
                {
                    //double maxTriggerTime=0.;
                    //for( int j=0 ; j<5 ; j++ )
                    //{
                    //if(  maxTriggerTime==0. || preMuonTriggetime[k][j]>maxTriggerTime )
                    //{
                    //maxTriggerTime=preMuonTriggetime[k][j];
                    //}
                    // 
                    //}
                    //preMuonTriggetime[k][5]=maxTriggerTime;

                    double minTriggerTime=0.;
                    for( int j=0 ; j<5 ; j++ )
                    {
                        if( (presentTriggerTime<nextMuonTriggetime[k][j]) && ( minTriggerTime==0. || nextMuonTriggetime[k][j]<minTriggerTime) )
                        {
                            minTriggerTime=nextMuonTriggetime[k][j];
                        }

                    }
                    nextMuonTriggetime[k][5]=minTriggerTime;
                }
            }
            for( int i=0 ; i<3 ; i++ )
            {
                cout<<" "<<endl;
                for( int j=0 ; j<6 ; j++ )
                {
                    cout<<T2lastMuon[j+16+6*i] <<" = "<<presentTriggerTime-preMuonTriggetime[i][j]<<" <- "<<presentTriggerTime <<" - "<<preMuonTriggetime[i][j]<<" : "<<T2lastMuon[34]<<" "<<T2lastMuon[35]<<" "<<T2lastMuon[36]<<endl;
                }
            }
        */

            for( int i=0 ; i<3 ; i++ )
            {
                for( int j=0 ; j<6 ; j++ )
                {
                    if( T2lastMuon[j+16+6*i]>=signalWinLow &&T2lastMuon[j+16+6*i]<=signalWinHigh )
                    {
                        IsignalWin[i][j]->Fill(energy);
                    }
                    if(T2lastMuon[j+34+6*i]>=offWinLow && T2lastMuon[j+34+6*i]<=offWinHigh)
                    {
                        IoffWin[i][j]->Fill(energy);
                    }
                }
            double mintime=1.e9;
            for( int j=0 ; j<3 ; j++ )
            {
                mintime=mintime>T2lastMuon[j+10+(i+1)*6]?T2lastMuon[j+10+(i+1)*6]:mintime;
            }
                    if( mintime>=signalWinLow &&mintime<=signalWinHigh )
                    {
                        IsignalWin[i][6]->Fill(energy);
                    }
                    double maxtime=-1.e9;
            for( int j=0 ; j<3 ; j++ )
            {
                if(T2lastMuon[j+28+(i+1)*6]<0.)maxtime=maxtime<T2lastMuon[j+28+(i+1)*6]?T2lastMuon[j+28+(i+1)*6]:maxtime;
            }
                    if(maxtime>=offWinLow && maxtime<=offWinHigh)
                    {
                        IoffWin[i][6]->Fill(energy);
                    }
            }
        }
    }
    if( energy<LowEdge || energy>HighEdge)
    {
        return 1; 
    }
    //AmC netron
    if( genAmCNeu )
    {
        if( z>0 )
        {
            singleUpper[det-1]->Fill(energy);
        }
        if( z<0 )
        {
            singleLower[det-1]->Fill(energy);
        }
    }
    if( genTimeFitDis )
    {

        for( int j=0 ; j<5 ; j++ )
        {
            time2lastshowermuon[j]->Fill(T2lastMuon[j+4]);
            time2lastshowermuonNoRed[j]->Fill(T2lastMuon[j+10]);

        }
        minT2allmuon[0]=1000.;
        minT2allmuon[1]=1000.;
        for( int j=0 ; j<5 ; j++ )
        {
            minT2allmuon[0]=minT2allmuon[0]<T2lastMuon[j+4]?minT2allmuon[0]:T2lastMuon[j+4];
            minT2allmuon[1]=minT2allmuon[1]<T2lastMuon[j+10]?minT2allmuon[1]:T2lastMuon[j+10];
        }
        time2Allmuon[0]->Fill(minT2allmuon[0]);
        time2Allmuon[1]->Fill(minT2allmuon[1]);
        for( int j=0 ; j<6 ; j++ )
        {
            time2lastmuon[j]->Fill();
            time2lastmuonNoRed[j]->Fill();
        }
        //TODO::fit time to last muon
        for( int j=0 ; j<3 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {
                Itime2lastshowermuon[j][i]->Fill(T2lastMuon[i+10+(j+1)*6]);
            }
            double mintime=1.e9;
            for( int i=0 ; i<3 ; i++ )
            {
                mintime=mintime>T2lastMuon[i+10+(j+1)*6]?T2lastMuon[i+10+(j+1)*6]:mintime;
            }
            Itime2lastshowermuon[j][6]->Fill(mintime);
        }
    }
    return kTRUE;
}

void SingleTree::SlaveTerminate()
{

}

void SingleTree::Terminate()
{
    std::cout<<"Now is in Terminate() "<<endl;
    if( genIsoSpec )
    {
        histname=Form("%s/EH%itotalHisto_%s.root",dataVer.Data(),site+1,dataVer.Data());
        TFile f(histname,"read");
        if( f.IsZombie() )
        {
            std::cout<<"Error : can't open "<<histname<<endl;
            return;
        }
        for( int j=0 ; j<2 ; j++ )
        {
            for( int i=0 ; i<6 ; i++ )
            {
                histname=Form("lidj/muonTimeInterval%s%i",ifRed[j].c_str(),i+1);
                TH1F* h=(TH1F*)f.Get(histname);
                if( !h )
                {
                    std::cout<<"Error : can't open TH1F "<<histname<<endl;
                    return;
                }
                file->cd();
                int offWinNum=(int)((offWinHigh-offWinLow)/(signalWinHigh-signalWinLow));
                //std::cout<<"offWinNum  : "<<offWinNum<<endl;

                //double offMuonRate=h->Integral(h->FindBin(offWinLow),h->FindBin(9999));
                //double offMuonRateH=h->Integral(h->FindBin(offWinHigh),h->FindBin(9999));

                int offWinLowBinNum=h->FindBin(offWinLow);
                double offMuonRate=h->GetBinContent(offWinLowBinNum)*((h->GetBinLowEdge(offWinLowBinNum+1)-offWinLow)/h->GetBinWidth(offWinLowBinNum))*(h->GetBinLowEdge(offWinLowBinNum+1)-offWinLow)/2;
                for( int l=offWinLowBinNum+1 ; l<h->FindBin(9999) ; l++ )
                {
                    offMuonRate=+h->GetBinContent(l)*(h->GetBinCenter(l)-offWinLow);
                }
                int offWinHighBinNumH=h->FindBin(offWinHigh);
                double offMuonRateH=h->GetBinContent(offWinHighBinNumH)*((h->GetBinLowEdge(offWinHighBinNumH+1)-offWinHigh)/h->GetBinWidth(offWinHighBinNumH))*(h->GetBinLowEdge(offWinHighBinNumH+1)-offWinHigh)/2;
                for( int l=offWinHighBinNumH+1 ; l<h->FindBin(9999) ; l++ )
                {
                    offMuonRateH=+h->GetBinContent(l)*(h->GetBinCenter(l)-offWinHigh);
                }

                //std::cout<<"offMuonRate  : "<<offMuonRate<<endl;
                double offFrac=0.;
                double offFracH=0.;
                for( int k=0 ; k<offWinNum ; k++ )
                {
                    //double offMuonRateTmp=h->Integral(h->FindBin(offWinLow+(signalWinHigh-signalWinLow)*k),h->FindBin(9999));
                    //double offMuonRateTmpH=h->Integral(h->FindBin(offWinHigh+(signalWinHigh-signalWinLow)*k),h->FindBin(9999));

                    int offWinLowBinNumTmp=h->FindBin(offWinLow+(signalWinHigh-signalWinLow)*k);
                    double offMuonRateTmp=h->GetBinContent(offWinLowBinNumTmp)*((h->GetBinLowEdge(offWinLowBinNumTmp+1)-offWinLow)/h->GetBinWidth(offWinLowBinNumTmp))*(h->GetBinLowEdge(offWinLowBinNumTmp+1)-offWinLow)/2;
                    for( int l=offWinLowBinNumTmp+1 ; l<h->FindBin(9999) ; l++ )
                    {
                        offMuonRateTmp=+h->GetBinContent(l)*(h->GetBinCenter(l)-offWinLow);
                    }
                    int offWinHighBinNumHTmp=h->FindBin(offWinHigh+(signalWinHigh-signalWinLow)*k);
                    double offMuonRateTmpH=h->GetBinContent(offWinHighBinNumHTmp)*((h->GetBinLowEdge(offWinHighBinNumHTmp+1)-offWinHigh)/h->GetBinWidth(offWinHighBinNumHTmp))*(h->GetBinLowEdge(offWinHighBinNumHTmp+1)-offWinHigh)/2;
                    for( int l=offWinHighBinNumHTmp+1 ; l<h->FindBin(9999) ; l++ )
                    {
                        offMuonRateTmpH=+h->GetBinContent(l)*(h->GetBinCenter(l)-offWinHigh);
                    }
                    //std::cout<<"offMuonRateTmp  : "<<offMuonRateTmp<<endl;
                    offFrac+=offMuonRateTmp/offMuonRate;
                    offFracH+=offMuonRateTmpH/offMuonRateH;
                }

                //double signalMuonRate=h->Integral(h->FindBin(signalWinLow),h->FindBin(9999));
                //double signalMuonRateH=h->Integral(h->FindBin(signalWinHigh),h->FindBin(9999));

                int signalWinLowBinNum=h->FindBin(signalWinLow);
                double signalMuonRate=h->GetBinContent(signalWinLowBinNum)*((h->GetBinLowEdge(signalWinLowBinNum+1)-signalWinLow)/h->GetBinWidth(signalWinLowBinNum))*(h->GetBinLowEdge(signalWinLowBinNum+1)-signalWinLow)/2;
                for( int l=signalWinLowBinNum+1 ; l<h->FindBin(9999) ; l++ )
                {
                    signalMuonRate=+h->GetBinContent(l)*(h->GetBinCenter(l)-signalWinLow);
                }
                int signalWinHighBinNumH=h->FindBin(signalWinHigh);
                double signalMuonRateH=h->GetBinContent(signalWinHighBinNumH)*((h->GetBinLowEdge(signalWinHighBinNumH+1)-signalWinHigh)/h->GetBinWidth(signalWinHighBinNumH))*(h->GetBinLowEdge(signalWinHighBinNumH+1)-signalWinHigh)/2;
                for( int l=signalWinHighBinNumH+1 ; l<h->FindBin(9999) ; l++ )
                {
                    signalMuonRateH=+h->GetBinContent(l)*(h->GetBinCenter(l)-signalWinHigh);
                }

                //std::cout<<" signalMuonRate : "<<signalMuonRate<<endl;
                std::cout<<"signalFrac  : "<<signalMuonRate/offMuonRate<<endl;
                std::cout<<"signalFracH  : "<<signalMuonRateH/offMuonRateH<<endl;
                cout<<"offFrac  : "<<offFrac<<endl;
                cout<<"offFracH  : "<<offFracH<<endl;
                histname=Form("%s %sSlice%i signalMuonRate:%10.0f offMuonRate:%10.0f signal/off:%5.5f offTheo/Real:%5.5f frac:%.5f",IsoMode.c_str(),ifRed[j].c_str(),i+1,signalMuonRate,offMuonRate,signalMuonRate/offMuonRate,(double)(offTheoNum[j][i])/offRealNum[j][i],offFrac);
                std::cout<<histname<<endl;
                delete h;
                //std::cout<<"1 "<<endl;
                //histname=Form("%sSpecColor",IsoMode.c_str());
                //c[j][i]=new TCanvas(histname,"IsoSpec",600,400);
                signalWin[j][i]->SetLineColor(kRed);
                signalWin[j][i]->GetXaxis()->SetTitle("Energy(MeV)");
                signalWin[j][i]->GetYaxis()->SetTitle("Entries");
                signalWin[j][i]->SetStats(kFALSE); 
                //signalWin[j][i]->Draw();
                offWin[j][i]->SetLineColor(kGreen);
                offWin[j][i]->SetStats(kFALSE); 
                //offWin[j][i]->Draw("same");
                histname=Form("%sSpec%sSlice%i_%0.1f_%0.1f",IsoMode.c_str(),ifRed[j].c_str(),i+1,LowEdge4e,HighEdge4e);
                histname2=histname+";*";
                gDirectory->Delete(histname2);
                isoSpec[j][i]=new TH1F(histname,histname,binNum,0,20);
                isoSpec[j][i]->Sumw2();
                //isoSpec[j][i]->Add(signalWin[j][i],offWin[j][i],1,-0.5);
                isoSpec[j][i]->Add(signalWin[j][i],offWin[j][i],1,-(1/(offFrac*offMuonRate/signalMuonRate*offRealNum[j][i]/offTheoNum[j][i])));
                isoSpec[j][i]->SetLineColor(kBlue);
                isoSpec[j][i]->SetStats(kFALSE);
                isoSpec[j][i]->SetMarkerStyle(20);
                isoSpec[j][i]->SetMarkerSize(0.7);
                isoSpec[j][i]->SetMarkerColor(kBlue);
                isoSpec[j][i]->SetOption("E");
                signalWin[j][i]->Write();
                offWin[j][i]->Write();
                isoSpec[j][i]->Write();
                singleSpecVsTime[j][i]->Write();
                file->cd();
                signalWinXY[j][i]->Write();
                offWinXY[j][i]->Write();
                signalWinRZ[j][i]->Write();
                offWinRZ[j][i]->Write();
                //c[j][i]->Write();

            }
        }

        for( int j=0 ; j<3 ; j++ )
        {
            histname=Form("I%dEnergySpec",j+1);
            TCanvas* cI=new TCanvas(histname,histname,2400,900);
            cI->Divide(6,4);
            for( int i=0 ; i<7 ; i++ )
            {
                cI->cd(i*3+1);
                IsignalWin[j][i]->Draw();
                cI->cd(i*3+2);
                IoffWin[j][i]->Draw();
                IisoSpec[j][i]->Sumw2();
                IisoSpec[j][i]->Add(IsignalWin[j][i],IoffWin[j][i],1,-1);
                cI->cd(i*3+3);
                IisoSpec[j][i]->Draw();
                
                IoffWin[j][i]->Write();
                IsignalWin[j][i]->Write();
                IisoSpec[j][i]->Write();

            }
            histname=Form("%s/EH%iI%dEnergySpec.eps",dataVer.Data(),site+1,j+1);
            cI->SaveAs(histname);
        }
        f.Close();
    }

    if( genAmCNeu )
    {

        TCanvas *c1=new TCanvas("AmCNeuSpec","AmCNeuSpec",1200,600);
        c1->Divide(2,2);
        for( int i=0 ; i<4 ; i++ )
        {
            c1->cd(i+1);
            singleUpper[i]->SetLineColor(kRed);
            singleUpper[i]->GetXaxis()->SetTitle("Energy(MeV)");
            singleUpper[i]->GetYaxis()->SetTitle("Entries/0.05Mev");
            singleUpper[i]->SetStats(kFALSE);
            //singleUpper[i]->Draw();
            singleLower[i]->SetLineColor(kGreen);
            singleLower[i]->SetStats(kFALSE);
            //singleLower[i]->Draw("same");
            AmCSpec[i]->Sumw2();
            AmCSpec[i]->Add(singleUpper[i],singleLower[i],1,-0.95);
            AmCSpec[i]->SetLineColor(kBlue);
            AmCSpec[i]->SetStats(kFALSE);
            //AmCSpec[i]->Draw("same");
            THStack* hs = new THStack("hs","");
            hs->Add(singleUpper[i]);
            hs->Add(singleLower[i]);
            //hs->Add(AmCSpec[i]);
            hs->Draw();
            AmCSpec[i]->Draw("same");
            TLegend *legend=new TLegend(.4,.65,.79,.89);
            //std::cout<<"singleUpper[i]->FindBin(LowEdge)  : "<<singleUpper[i]->FindBin(LowEdge)<<endl;
            //std::cout<<"singleUpper[i]->FindBin(HighEdge)  : "<<singleUpper[i]->FindBin(HighEdge)<<endl;
            double upperNum=singleUpper[i]->Integral(singleUpper[i]->FindBin(LowEdge),singleUpper[i]->FindBin(HighEdge));
            double lowNum=singleLower[i]->Integral(singleLower[i]->FindBin(LowEdge),singleLower[i]->FindBin(HighEdge));
            histname=Form("Single Upper : %0.0f",upperNum);
            legend->AddEntry(singleUpper[i],histname,"lp");
            histname=Form("Single Lower : %0.0f",lowNum);
            legend->AddEntry(singleLower[i],histname,"lp");
            histname=Form("AmC number : %0.0f",upperNum-lowNum*0.95);
            legend->AddEntry(AmCSpec[i],histname,"lp");
            histname=Form("   %0.1f ~ %0.1f MeV",LowEdge,HighEdge);
            legend->SetHeader(histname);
            legend->SetFillColor(0);
            legend->Draw("same");
            /*
               for( int j=81 ; j<401 ; j++ )
               {
               if( singleUpper[i]->GetBinContent(j)==0 )
               {
               continue;
               }
               std::cout<<"Upper "<<i<<"  : "<<singleUpper[i]->GetBinContent(j)<<endl;
               std::cout<<"Lower "<<i<<"  : "<<singleLower[i]->GetBinContent(j)<<endl;
               std::cout<<"AmC   "<<i<<"  : "<<AmCSpec[i]->GetBinContent(j)<<endl;
               std::cout<<"U-L   "<<i<<"  : "<<singleUpper[i]->GetBinContent(j)-singleLower[i]->GetBinContent(j)*0.95<<endl;
               }
               */
        }

        for( int i=0 ; i<4 ; i++ )
        {
            singleUpper[i]->Write();
            singleLower[i]->Write();
            AmCSpec[i]->Write();
        }
        c1->Write();
    }
    if( genTimeFitDis )
    {
        for( int i=0 ; i<5 ; i++ )
        {
            time2lastshowermuon[i]->Write();
            time2lastshowermuonNoRed[i]->Write();
        }
        time2Allmuon[0]->Write();
        time2Allmuon[1]->Write();
        for( int j=0 ; j<3 ; j++ )
        {
            for( int i=0 ; i<7 ; i++ )
            {
                Itime2lastshowermuon[j][i]->Write();
            }
        }
    }

    //c1->Write();
    file->Close();
}
