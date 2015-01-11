#include "IsotopesAna.h"
#include "LafKernel/AlgFactory.h"
#include  <algorithm>
#include  <fstream>

DECLARE_ALGORITHM(IsotopesAna);

    IsotopesAna::IsotopesAna(const std::string& name)
: AlgBase(name)
{
    OptionParser::setOption(name,"EnergyLow",ELow);
    OptionParser::setOption(name,"EnergyHigh",EHigh);
    OptionParser::setOption(name,"AdMuonELow",AdMuonELow);
    OptionParser::setOption(name,"AdMuonEHigh",AdMuonEHigh);
    OptionParser::setOption(name,"Time2LastBufEvent",Time2LastBufEvent);
    OptionParser::setOption(name,"StatMuonTimeInterval",statMuonTimeInterval=1);

    lastOwpMuonTrigtime.SetSec(0);
    lastOwpMuonTrigtime.SetNanoSec(0);
    lastIwpMuonTrigtime.SetSec(0);
    lastIwpMuonTrigtime.SetNanoSec(0);


    for( int i=0 ; i<GlobalVar::NumADs ; i++ )
    {
        AdEvtBuf[i].clear();
        time2MuonBuf[i].clear();
        for( int j=0 ; j<6 ; j++ )
        {
            lastshowermuonTrigtime[j][i].SetSec(0);
            lastshowermuonTrigtime[j][i].SetNanoSec(0);
            lastshowermuonTrigtimeTmp[j][i].SetSec(0);
            lastshowermuonTrigtimeTmp[j][i].SetNanoSec(0);
            lastshowermuonTrigtimeVec[j][i].clear();
            muonEnergyVec[j][i].clear();
            lastshowermuonTrigtimeNoRed[j][i].SetSec(0);
            lastshowermuonTrigtimeNoRed[j][i].SetNanoSec(0);
            for( int k=0 ; k<4 ; k++ )
            {
                finalTestADMuon[k][j][i]=NULL;
                adMuonTriggerTimeBuf[k][j][i].clear();
                adMuonEnergyBuf[k][j][i].clear();
                lastshowermuonTrigtimeI[k][j][i].SetSec(0);
                lastshowermuonTrigtimeI[k][j][i].SetNanoSec(0);
                nextImuonTriggerTime[k][j][i]=0.;
            }

        }

        lastAdMuonTrigtime[i].SetSec(0);
        lastAdMuonTrigtime[i].SetNanoSec(0);
        lastShowerMuonTrigtime[i].SetSec(0);
        lastShowerMuonTrigtime[i].SetNanoSec(0);
    }

    saveSingle=true;
}

bool IsotopesAna::initialize()
{
    EvtBuffer = dynamic_cast<PhyEventBuf*>(service("Cycler"));
    liveTimeSvc = dynamic_cast<LiveTimeSvc*>(service("LiveTimeSvc"));
    muonVeto_l = MuonVeto::instance();

    for( int j=0 ; j<7 ; j++ )
    {
        ADMuonNum[j]=0;
    }

    for( int j=0 ; j<6 ; j++ )
    {
        histName="time2lastshowermuon";
        histName+=j+1;
        time2lastshowermuon[j] = new TH1F(histName,"time2lastshowermuon",100000,0.,100.); 
        ntupleSvc()->attach("FILE1/lidj",time2lastshowermuon[j]);
        histName="showermuonNum";
        histName+=j+1;
        showermuonNum[j] = new TH1F(histName,"number of AD showermuon",liveTimeSvc->nBins(),liveTimeSvc->startTime().AsDouble(),liveTimeSvc->endTime().AsDouble());
        ntupleSvc()->attach("FILE1/lidj",showermuonNum[j]);
        histName="muonEnergy";
        histName+=j+1;
        muonEnergy[j] = new TH1F(histName,"energy of AD muon",10000,0,10000);
        ntupleSvc()->attach("FILE1/lidj",muonEnergy[j]);

        for( int k=0 ; k<4 ; k++ )
        {
            histName="time2lastshowermuonI";
            histName+=k+1;
            histName+=j+1;
            time2lastshowermuonI[k][j] = new TH1F(histName,"time2lastshowermuon",100000,0.,100.); 
            ntupleSvc()->attach("FILE1/lidj",time2lastshowermuonI[k][j]);
            histName="showermuonNumI";
            histName+=k+1;
            histName+=j+1;
            showermuonNumI[k][j] = new TH1F(histName,"number of AD showermuon",liveTimeSvc->nBins(),liveTimeSvc->startTime().AsDouble(),liveTimeSvc->endTime().AsDouble());
            ntupleSvc()->attach("FILE1/lidj",showermuonNumI[k][j]);
            histName="muonEnergyI";
            histName+=k+1;
            histName+=j+1;
            muonEnergyI[k][j] = new TH1F(histName,"energy of AD muon",10000,0,10000);
            ntupleSvc()->attach("FILE1/lidj",muonEnergyI[k][j]);
        }

        histName="time2lastshowermuonNoRed";
        histName+=j+1;
        time2lastshowermuonNoRed[j] = new TH1F(histName,"time2lastshowermuonNoRed",100000,0.,100.); 
        ntupleSvc()->attach("FILE1/lidj",time2lastshowermuonNoRed[j]);
        histName="showermuonNumNoRed";
        histName+=j+1;
        showermuonNumNoRed[j] = new TH1F(histName,"number of AD showermuon",liveTimeSvc->nBins(),liveTimeSvc->startTime().AsDouble(),liveTimeSvc->endTime().AsDouble());
        ntupleSvc()->attach("FILE1/lidj",showermuonNumNoRed[j]);
        histName="muonEnergyNoRed";
        histName+=j+1;
        muonEnergyNoRed[j] = new TH1F(histName,"energy of AD muon",10000,0,10000);
        ntupleSvc()->attach("FILE1/lidj",muonEnergyNoRed[j]);

        if( statMuonTimeInterval )
        {
            histName="muonTimeIntervalNoRed";
            histName+=j+1;
            muonTimeIntervalNoRed[j] = new TH1F(histName,"time interval between muon in each slice",10000000,0.,10000.);
            ntupleSvc()->attach("FILE1/lidj",muonTimeIntervalNoRed[j]);
            histName="muonTimeInterval";
            histName+=j+1;
            muonTimeInterval[j] = new TH1F(histName,"time interval between muon in each slice",10000000,0.,10000.);
            ntupleSvc()->attach("FILE1/lidj",muonTimeInterval[j]);
        }
    }

    time2Allmuon=new TH1F("time2Allmuon","time2Allmuon",100000,0.,100.);
    ntupleSvc()->attach("FILE1/lidj",time2Allmuon);
    for( int i=0 ; i<4 ; i++ )
    {
        histName="t2nextEvtAD";
        histName+=i+1;
        t2nextEvt[i]=new TH1F(histName,histName,90,2.e-4,2.e-3);
        ntupleSvc()->attach("FILE1/lidj",t2nextEvt[i]);
    }


    if( saveSingle )
    {
        SingleTree = ntupleSvc()->bookTree("FILE2/Tree/SingleTree","SingleTree");
        SingleTree->Branch("energy",&energy,"energy/F");
        SingleTree->Branch("x",&x,"x/F");
        SingleTree->Branch("y",&y,"y/F");
        SingleTree->Branch("z",&z,"z/F");
        SingleTree->Branch("det",&det,"det/I");
        SingleTree->Branch("triggerTime",&triggerTime,"triggerTime/D");
        SingleTree->Branch("t2lastshowermuon",&t2lastshowermuon,"t2lastshowermuon/D");
        SingleTree->Branch("T2lastMuon",T2lastMuon,"T2lastMuon[64]/D");
    }

    std::cout<<"IsotopesAna initializing..."<<endl;
    return true;
}

bool IsotopesAna::execute()
{

    PhyEvent* CurEvent= EvtBuffer->curEvt();

    if( !CurEvent->m_good )
    {
        return true;
    }
    if( CurEvent->isAdMuon() )
    {
        ADMuonNum[0]++;
    }
    if( CurEvent->isMuon())
    {
        //need to delete this function "updateVetoWindow",in "NtupleAna/Algorithms/src/MuonTagAlg.cc" ,it has been executed once.If you don't use the project"MuonTagAlg",you should not delete this line.
        //muonVeto_l->updateVetoWindow(CurEvent);

        if( CurEvent->isWpMuon() && CurEvent->m_det==5 )
        {
            lastIwpMuonTrigtime.SetSec(CurEvent->m_trigTime.GetSec());
            lastIwpMuonTrigtime.SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
        } else if(CurEvent->isWpMuon() && CurEvent->m_det==6)
        {
            lastOwpMuonTrigtime.SetSec(CurEvent->m_trigTime.GetSec());
            lastOwpMuonTrigtime.SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
        } else if( CurEvent->isAdMuon() )
        {
            lastAdMuonTrigtime[CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
            lastAdMuonTrigtime[CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
        }else if(CurEvent->isShowerMuon())
        {
            lastShowerMuonTrigtime[CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
            lastShowerMuonTrigtime[CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
        }else
        {
            // 
        }
        if( !(CurEvent->isAD()) )
        {
            return true;
        }

        float AdMuonEnergy=0.;//CurEvent->energy();
        AdMuonEnergy=CurEvent->energy();

        //the sixth for all muons
        lastshowermuonTrigtimeVec[5][CurEvent->m_det-1].push_back(CurEvent->m_trigTime);
        muonEnergyVec[5][CurEvent->m_det-1].push_back(AdMuonEnergy);
        ADMuonNum[6]++;
        if(statMuonTimeInterval&&lastshowermuonTrigtimeNoRed[5][CurEvent->m_det-1].GetSec()!=0.) muonTimeIntervalNoRed[5]->Fill(CurEvent->m_trigTime-lastshowermuonTrigtimeNoRed[5][CurEvent->m_det-1]);
        lastshowermuonTrigtimeNoRed[5][CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
        lastshowermuonTrigtimeNoRed[5][CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
        showermuonNumNoRed[5]->Fill(CurEvent->m_trigTime);
        muonEnergyNoRed[5]->Fill(AdMuonEnergy);

        //pre AD muon
        PhyEvent* preADMuon=muonVeto_l->preMuon(CurEvent,CurEvent->m_det);
        double time2preADMuon=1000000.;
        if( preADMuon )
        {
            time2preADMuon=CurEvent->m_trigTime - preADMuon->m_trigTime;
        } 
        //next AD muon
        PhyEvent* nextADMuon=muonVeto_l->nextMuon(CurEvent,CurEvent->m_det);
        double time2nextADMuon=1000000.;
        if( nextADMuon )
        {
            time2nextADMuon=nextADMuon->m_trigTime - CurEvent->m_trigTime;
        } 

        for( int k=0 ; k<4 ; k++ )
        {
            if( (k<3 && time2preADMuon>=(0.1*(k+1)) && time2nextADMuon>=(0.1*(k+1))) || (k==3 && time2preADMuon>=0.2) )
            {
                lastshowermuonTrigtimeI[k][5][CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
                lastshowermuonTrigtimeI[k][5][CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
                showermuonNumI[k][5]->Fill(lastshowermuonTrigtimeI[k][5][CurEvent->m_det-1]);
                muonEnergyI[k][5]->Fill(AdMuonEnergy);
                i4num1[k]++;
            }

        }



        float AdMuonEnergyThr[6]={20,60,500,1500,2500,10000000};
        for( int l=0 ; l<5 ; l++ )
        {
            if( AdMuonEnergy>=AdMuonEnergyThr[l] && AdMuonEnergy< AdMuonEnergyThr[l+1] )
            {
                lastshowermuonTrigtimeVec[l][CurEvent->m_det-1].push_back(CurEvent->m_trigTime);
                muonEnergyVec[l][CurEvent->m_det-1].push_back(AdMuonEnergy);
                ADMuonNum[l+1]++;

                for( int k=0 ; k<4 ; k++ )
                {
                    if( (k<3 && time2preADMuon>=(0.1*(k+1)) && time2nextADMuon>=(0.1*(k+1))) || (k==3 && time2preADMuon>=0.2) )
                    {
                        lastshowermuonTrigtimeI[k][l][CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
                        lastshowermuonTrigtimeI[k][l][CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
                        showermuonNumI[k][l]->Fill(lastshowermuonTrigtimeI[k][l][CurEvent->m_det-1]);
                        muonEnergyI[k][l]->Fill(AdMuonEnergy);
                    }

                }

                if(statMuonTimeInterval&&lastshowermuonTrigtimeNoRed[l][CurEvent->m_det-1].GetSec()!=0.) muonTimeIntervalNoRed[l]->Fill(CurEvent->m_trigTime-lastshowermuonTrigtimeNoRed[l][CurEvent->m_det-1]);
                lastshowermuonTrigtimeNoRed[l][CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
                lastshowermuonTrigtimeNoRed[l][CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
                showermuonNumNoRed[l]->Fill(CurEvent->m_trigTime);
                muonEnergyNoRed[l]->Fill(AdMuonEnergy);
            } 
        }

        return true;

    }

    if( !CurEvent->isAD())
    {
        return true;
    }

    bool isNeulike=false;
    if( CurEvent->energy()>1.8 && CurEvent->energy()<12.0 )
    {
        isNeulike=true;
    } 
    /*
       else if( CurEvent->energy()>1.8 && CurEvent->energy()<2.8 )
       {
       isNeulike=true;
       }else
       {
    //
    }
    */
    if(isNeulike) 
    {

        //for( int i=0 ; i<6 ; i++ )
        for( int i=0 ; i<6 ; i++ )
        {
            if( !lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].empty() )
            {
                vector<TTimeStamp>::iterator it=lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].end();
                it--;
                vector<float>::iterator itE=muonEnergyVec[i][CurEvent->m_det-1].end();
                itE--;
                bool lastAdmuon=true;
                lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].SetSec(0);
                lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].SetNanoSec(0);
                for(; it>=lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].begin();it--) 
                {
                    if( (CurEvent->m_trigTime-*it)>1.e-5 &&(CurEvent->m_trigTime-*it)<2.e-4)
                    {
                        if( lastAdmuon )
                        {
                            lastlastshowermuonTrigtime[i][CurEvent->m_det-1].SetSec(lastshowermuonTrigtime[i][CurEvent->m_det-1].GetSec());
                            lastlastshowermuonTrigtime[i][CurEvent->m_det-1].SetNanoSec(lastshowermuonTrigtime[i][CurEvent->m_det-1].GetNanoSec());
                            lastshowermuonTrigtime[i][CurEvent->m_det-1].SetSec((*it).GetSec());
                            lastshowermuonTrigtime[i][CurEvent->m_det-1].SetNanoSec((*it).GetNanoSec());
                            lastAdmuon=false;
                        }
                        if(statMuonTimeInterval&&lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].GetSec()!=0.) muonTimeInterval[i]->Fill(fabs(lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1]-*it));
                        lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].SetSec((*it).GetSec());
                        lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].SetNanoSec((*it).GetNanoSec());
                        showermuonNum[i]->Fill(lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1]);
                        muonEnergy[i]->Fill(*itE);
                        it=lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].erase(it);
                        itE=muonEnergyVec[i][CurEvent->m_det-1].erase(itE);
                    } else if( (CurEvent->m_trigTime-*it)>=2.e-4 )
                    {
                        it++;
                        lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].erase(lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].begin(),it); 
                        itE++;
                        muonEnergyVec[i][CurEvent->m_det-1].erase(muonEnergyVec[i][CurEvent->m_det-1].begin(),itE); 
                        break;
                    } else
                    {
                        //
                    }
                    itE--;
                }
                if(statMuonTimeInterval&&lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].GetSec()!=0.&&lastlastshowermuonTrigtime[i][CurEvent->m_det-1].GetSec()!=0.) muonTimeInterval[i]->Fill(fabs(lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1]-lastlastshowermuonTrigtime[i][CurEvent->m_det-1]));
            }
        }

    }


    int vetotag=muonVeto_l->veto(CurEvent);

    if( vetotag==1 )//1 for muon veto,except RPC,2 only for RPC veto,0 not be vetoed by muons
    {
        return true;
    } 

    if( !(CurEvent->energy()>0.7) )
    {
        return true;
    }

    dump(CurEvent->m_det,0,CurEvent);
    AdEvtBuf[CurEvent->m_det-1].push_back(CurEvent->GrabInstance());


    CalTime2Muon(CurEvent);
    time2MuonBuf[CurEvent->m_det-1].push_back(time2MuonVec);

    return true;

}

bool IsotopesAna::finalize()
{
    PhyEvent* CurEvent=0;
    for( int i=1 ; i<5 ; i++ )
    {
        dump(i,1,CurEvent);
    }

    std::cout<<"IsotopesAna finalize..."<<endl;
    for( int i=0 ; i<7 ; i++ )
    {
        std::cout<<"ADmuon number "<<i <<" : "<<ADMuonNum[i]<<endl;
    }
    for( int k=0 ; k<4 ; k++ )
    {
        cout<<"i4num1  : "<<i4num1[k]<<" i4num2  : "<<i4num2[k]<<endl;
    }
    

    return true;
}

bool IsotopesAna::FillSingle(PhyEvent* Evt)
{
    if( time2MuonBuf[Evt->m_det-1].size()!=1 )
    {
        std::cout<<"wrong !! time2MuonBuf["<<Evt->m_det-1<<"].size()="<<time2MuonBuf[Evt->m_det-1].size() <<endl;
    }
    energy=Evt->energy();
    if( energy>EHigh || energy<ELow)
        //if( energy>12. || energy<6.)
    {
        return true;
    }

    /*
       if( ((time2MuonBuf[Evt->m_det-1][0][0]>=DelayedTrigTime2AdMuon)&&(time2MuonBuf[Evt->m_det-1][0][2]>=DelayedTrigTime2IWpMuon)&&(time2MuonBuf[Evt->m_det-1][0][1]>=DelayedTrigTime2AdShowerMuon)&&(time2MuonBuf[Evt->m_det-1][0][3]>=DelayedTrigTime2OWpMuon)) )
       {
       }
       */
    for( int i=0 ; i<6 ; i++ )
    {
        time2lastshowermuon[i]->Fill(time2MuonBuf[Evt->m_det-1][0][i+4]);
        for( int k=0 ; k<3 ; k++ )
        {
            time2lastshowermuonI[k][i]->Fill(time2MuonBuf[Evt->m_det-1][0][i+10+(k+1)*6]);
        }

        time2lastshowermuonNoRed[i]->Fill(time2MuonBuf[Evt->m_det-1][0][i+10]);
    }
    time2Allmuon->Fill(time2MuonBuf[Evt->m_det-1][0][1]);

    DataBuffer<PhyEvent>::Iterator it=EvtBuffer->find(Evt);
    it++;
    while ( it < EvtBuffer->end() ) 
    {
        PhyEvent* nextEvt = (*it);
        if( nextEvt->m_good && nextEvt->isAD() && nextEvt->energy()>0.7 )
        {
            if( (nextEvt->energy()>1.8)&&(nextEvt->energy()<3.)&&(nextEvt->m_trigTime-Evt->m_trigTime>2.e-4) )
            {
                t2nextEvt[Evt->m_det-1]->Fill(nextEvt->m_trigTime-Evt->m_trigTime);
            }
            break;
        }
        it++;
    }
    if( ((Evt->m_quadrant>0.5)&&(Evt->m_maxQ>0.03)) )
    {
        std::cout<<" "<<endl;
        std::cout<<"Evt->m_quadrant  : "<<Evt->m_quadrant<<endl;
        std::cout<<"Evt->m_maxQ  : "<<Evt->m_maxQ<<endl;
        return true;
    }

    //************************

    for(int i=0;i<64;i++) T2lastMuon[i]=10.e6;
    for(int i=0;i<40;i++) T2lastMuon[i]=time2MuonBuf[Evt->m_det-1][0][i];

    float AdMuonEnergyThr[6]={20,60,500,1500,2500,10000000};
    int detTmp=Evt->m_det-1;
    for(int k=0;k<4;k++)
    {
        bool needUpdate6=0;
        for( int t=0 ; t<5 ; t++ )
        {
            if( nextImuonTriggerTime[k][t][detTmp]<=Evt->m_trigTime.AsDouble() )
            {
                needUpdate6=1;
                PhyEvent* nextADMuon=NULL;
                double lastAdmuonTrigTime=0.;
                if( finalTestADMuon[k][t][detTmp] && finalTestADMuon[k][t][detTmp]->m_trigTime>Evt->m_trigTime && ((k<3&&finalTestADMuon[k][t][detTmp]->m_trigTime-Evt->m_trigTime<=0.2*(k+1)) || (k==3&&finalTestADMuon[k][t][detTmp]->m_trigTime-Evt->m_trigTime<=0.2) ))
                {
                    nextADMuon=muonVeto_l->nextMuon(finalTestADMuon[k][t][detTmp],Evt->m_det);
                    lastAdmuonTrigTime=finalTestADMuon[k][t][detTmp]->m_trigTime.AsDouble();
                }else
                {
                    adMuonTriggerTimeBuf[k][t][detTmp].clear();
                    adMuonEnergyBuf[k][t][detTmp].clear();
                    double lastTrigtime=Evt->m_trigTime.AsDouble()-(T2lastMuon[0]>T2lastMuon[1]?T2lastMuon[1]:T2lastMuon[0]);
                    adMuonTriggerTimeBuf[k][t][detTmp].push_back(lastTrigtime);
                    lastAdmuonTrigTime=lastTrigtime;
                    adMuonEnergyBuf[k][t][detTmp].push_back(15);
                    nextADMuon=muonVeto_l->nextMuon(Evt,Evt->m_det);
                }
                bool doLoop=1;
                bool findOut=0;
                while(doLoop)
                {
                    if( !adMuonTriggerTimeBuf[k][t][detTmp].empty() )
                    {
                        if(adMuonTriggerTimeBuf[k][t][detTmp].size()!=adMuonEnergyBuf[k][t][detTmp].size()) cout<<"Error :  adMuonTriggerTimeBuf[k][t][detTmp].size()!=adMuonEnergyBuf[k][t][detTmp].size()"<<endl;
                        double LastEventTriggerTime=adMuonTriggerTimeBuf[k][t][detTmp][adMuonTriggerTimeBuf[k][t][detTmp].size()-1];
                        if( !nextADMuon || (k<3 && nextADMuon->m_trigTime.AsDouble()-LastEventTriggerTime>=0.1*(k+1))||(k==3))
                        {
                            if( adMuonTriggerTimeBuf[k][t][detTmp].size()==1 && LastEventTriggerTime>Evt->m_trigTime.AsDouble())
                            {
                                //find one
                                if( adMuonEnergyBuf[k][t][detTmp][0]>AdMuonEnergyThr[t] && adMuonEnergyBuf[k][t][detTmp][0]<AdMuonEnergyThr[t+1])
                                {
                                    nextImuonTriggerTime[k][t][detTmp]=LastEventTriggerTime;
                                    i4num2[k]++;
                                    findOut=1;
                                }
                            }
                            adMuonTriggerTimeBuf[k][t][detTmp].clear();
                            adMuonEnergyBuf[k][t][detTmp].clear();
                        }
                    }
                    doLoop=0;
                    if( nextADMuon)
                    {
                        if( k<3||(k==3 &&nextADMuon->m_trigTime.AsDouble()-lastAdmuonTrigTime>=0.2) )
                        {
                            adMuonTriggerTimeBuf[k][t][detTmp].push_back(nextADMuon->m_trigTime.AsDouble());
                            adMuonEnergyBuf[k][t][detTmp].push_back(nextADMuon->energy());
                        }
                        finalTestADMuon[k][t][detTmp]=nextADMuon->GrabInstance();
                        if( (k<3 && nextADMuon->m_trigTime-Evt->m_trigTime<=0.2*(k+1)) ||(k==3 && nextADMuon->m_trigTime-Evt->m_trigTime<=0.2) )
                        {
                            lastAdmuonTrigTime=nextADMuon->m_trigTime.AsDouble();
                            nextADMuon=muonVeto_l->nextMuon(nextADMuon,Evt->m_det);
                            doLoop=!findOut;
                        }
                    }
                } 
            }
            if(nextImuonTriggerTime[k][t][detTmp]!=0) T2lastMuon[t+k*6+40]=Evt->m_trigTime.AsDouble()-nextImuonTriggerTime[k][t][detTmp];
        }
        if( needUpdate6 )
        {
            double minTriggerTime=0.;
            for( int i=0 ; i<5 ; i++ )
            {
                if(nextImuonTriggerTime[k][i][detTmp]>Evt->m_trigTime.AsDouble()&&(nextImuonTriggerTime[k][i][detTmp]<minTriggerTime || minTriggerTime==0.)) minTriggerTime=nextImuonTriggerTime[k][i][detTmp];
            }
            if( minTriggerTime!=0. )
            {
                nextImuonTriggerTime[k][5][detTmp]=minTriggerTime;
            }

        }
        if(nextImuonTriggerTime[k][5][detTmp]!=0) T2lastMuon[5+k*6+40]=Evt->m_trigTime.AsDouble()-nextImuonTriggerTime[k][5][detTmp];

    }


    //************************




    if( saveSingle )
    {
        det=Evt->m_det;
        x=Evt->m_x;
        y=Evt->m_y;
        z=Evt->m_z;
        triggerTime=Evt->m_trigTime; 
        //CalTime2Muon(Evt);
        t2lastshowermuon=time2MuonBuf[Evt->m_det-1][0][1];
        SingleTree->Fill();
    }
    //std::cout<<"Single energy: "<<endl;
    //std::cout<<"    "<<Evt->energy()<<endl;
    //std::cout<<"time to last muon  : "<<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][10]<<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][11]<<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][12]<<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][13]<<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][14]<<endl;
    //std::cout<<"time to AD ADShower IWS OWS : "<<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][0] <<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][1] <<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][2] <<endl;
    //std::cout<<"   "<<time2MuonBuf[Evt->m_det-1][0][3] <<endl;

    return true;
}

bool IsotopesAna::CalTime2Muon(PhyEvent* event)
{
    //can not be used in dump().must only be used for current event,because these last*muonTrigtimes are latest muons!!!
    time2MuonVec.clear();
    for(int i=0;i<64;i++) time2Muon[i]=10.e6;
    //pre ADMuon
    if( lastAdMuonTrigtime[event->m_det-1].GetSec()!=0. ) time2Muon[0]=event->m_trigTime-lastAdMuonTrigtime[event->m_det-1];
    //pre ADshower muon
    if( lastShowerMuonTrigtime[event->m_det-1].GetSec()!=0. ) time2Muon[1]=event->m_trigTime - lastShowerMuonTrigtime[event->m_det-1];
    //pre Iwp muon
    if( lastIwpMuonTrigtime.GetSec()!=0. ) time2Muon[2]=event->m_trigTime - lastIwpMuonTrigtime;
    //pre Owp muon
    if( lastOwpMuonTrigtime.GetSec()!=0. ) time2Muon[3]=event->m_trigTime - lastOwpMuonTrigtime;
    //for B12/N12
    for( int i=0 ; i<6 ; i++ )
    {
        if( lastshowermuonTrigtime[i][event->m_det-1].GetSec()!=0. ) time2Muon[i+4]=event->m_trigTime - lastshowermuonTrigtime[i][event->m_det-1];
        for( int k=0 ; k<4 ; k++ )
        {
            if( lastshowermuonTrigtimeI[k][i][event->m_det-1].GetSec()!=0. ) time2Muon[i+16+k*6]=event->m_trigTime - lastshowermuonTrigtimeI[k][i][event->m_det-1];
            if(nextImuonTriggerTime[k][i][event->m_det-1]!=0.)
            {
                time2Muon[i+40+k*6]=event->m_trigTime.AsDouble() - nextImuonTriggerTime[k][i][event->m_det-1]; 
            }
        }
        if( lastshowermuonTrigtimeNoRed[i][event->m_det-1].GetSec()!=0. ) time2Muon[i+10]=event->m_trigTime - lastshowermuonTrigtimeNoRed[i][event->m_det-1];
    }

    //last muon I trigger time


    for( int i=0 ; i<64 ; i++ ) time2MuonVec.push_back(time2Muon[i]);
    return true;
}


bool IsotopesAna::printEvt(PhyEvent* CurEvent)
{
    std::cout<<"===> info <==="<<endl;
    std::cout<<"entry = "<<CurEvent->m_entry<<endl;
    std::cout<<"fileNum = "<<CurEvent->m_fileNum<<endl;
    std::cout<<"localentry = "<<CurEvent->m_localEntry<<endl;
    std::cout<<"trigtime = "<<CurEvent->m_trigTime<<endl;
    std::cout<<"trigType = "<<CurEvent->m_trigType<<endl;
    std::cout<<"det = "<<CurEvent->m_det<<endl;
    std::cout<<"energy = "<<CurEvent->m_energy<<endl;
    std::cout<<"x = "<<CurEvent->m_x<<endl;
    std::cout<<"y = "<<CurEvent->m_y<<endl;
    std::cout<<"z = "<<CurEvent->m_z<<endl;
    std::cout<<"rawEvis = "<<CurEvent->m_rawEvis<<endl;
    std::cout<<"energy() = "<<CurEvent->energy()<<endl;
    std::cout<<"nPmt = "<<CurEvent->m_nPmt<<endl;
    std::cout<<"flasherTag = "<<CurEvent->m_flasherTag<<endl;
    std::cout<<"forceTrigTag = "<<CurEvent->m_forceTrigTag<<endl;
    std::cout<<"crossTrigTag = "<<CurEvent->m_crossTrigTag<<endl;
    std::cout<<"rpcNoiseTag = "<<CurEvent->m_rpcNoiseTag<<endl;
    std::cout<<"adLowEnergyTag = "<<CurEvent->m_adLowEnergyTag<<endl;
    std::cout<<"goodevent = "<<CurEvent->m_good<<endl;
    std::cout<<"muontag = "<<CurEvent->m_muonTag<<endl;
    return true;
}

void IsotopesAna::dump(int i_det,bool IsFinal,PhyEvent* CurEvent)
{
    if( !AdEvtBuf[i_det-1].empty() )
    {
        PhyEvent* LastEvent=AdEvtBuf[i_det-1][AdEvtBuf[i_det-1].size()-1];
        if( IsFinal || (CurEvent->m_trigTime-LastEvent->m_trigTime)>Time2LastBufEvent )
            //if( IsFinal || (CurEvent->m_trigTime-LastEvent->m_trigTime)>0)
        {
            if( AdEvtBuf[i_det-1].size()==1 )
            {
                FillSingle(AdEvtBuf[i_det-1][0]);
            } else if(AdEvtBuf[i_det-1].size()==2)
            {
                //FillLi(AdEvtBuf[i_det-1]);
            }else
            {
                //
            }
            for( int i=0 ; i<(int)AdEvtBuf[i_det-1].size();i++)
            {
                AdEvtBuf[i_det-1][i]->ReleaseInstance();
            }

            AdEvtBuf[i_det-1].clear(); 
            time2MuonBuf[i_det-1].clear();
        }
    } 

}
