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
	OptionParser::setOption(name,"PromptEnergyLow4Li",promptELow4Li);
	OptionParser::setOption(name,"PromptEnergyHigh4Li",promptEHigh4Li);
	OptionParser::setOption(name,"DelayedEnergyLow4Li",delayedELow4Li);
	OptionParser::setOption(name,"DelayedEnergyHigh4Li",delayedEHigh4Li);
	OptionParser::setOption(name,"AdMuonELow",AdMuonELow);
	OptionParser::setOption(name,"AdMuonEHigh",AdMuonEHigh);
	OptionParser::setOption(name,"LiIntervalMin",LiIntervalMin);
	OptionParser::setOption(name,"LiIntervalMax",LiIntervalMax);
	OptionParser::setOption(name,"Time2LastBufEvent",Time2LastBufEvent);
	OptionParser::setOption(name,"DelayedTrigTime2AdMuon4Li",DelayedTrigTime2AdMuon4Li=1.e-3);
	OptionParser::setOption(name,"DelayedTrigTime2IWpMuon4Li",DelayedTrigTime2IWpMuon4Li=6.e-4);
	OptionParser::setOption(name,"DelayedTrigTime2OWpMuon4Li",DelayedTrigTime2OWpMuon4Li=6.e-4);
	OptionParser::setOption(name,"DelayedTrigTime2AdShowerMuon4Li",DelayedTrigTime2AdShowerMuon4Li=1.e-3);

    lastOwpMuonTrigtime.SetSec(0);
    lastOwpMuonTrigtime.SetNanoSec(0);
    lastIwpMuonTrigtime.SetSec(0);
    lastIwpMuonTrigtime.SetNanoSec(0);

    for( int i=0 ; i<=980 ; i++ )
    {
        energySlice[i]=20+i;
    }
    for( int i=981 ; i<MAXNUM+1 ; i++ )
    {
        energySlice[i]=1000+(i-980)*5;
    }
    
    
	for( int i=0 ; i<GlobalVar::NumADs ; i++ )
	{
		AdEvtBuf[i].clear();
        time2MuonBuf[i].clear();
        for( int j=0 ; j<MAXNUM ; j++ )
        {
            lastshowermuonTrigtime[j][i].SetSec(0);
            lastshowermuonTrigtime[j][i].SetNanoSec(0);
            lastshowermuonTrigtimeTmp[j][i].SetSec(0);
            lastshowermuonTrigtimeTmp[j][i].SetNanoSec(0);
            tAndeVec[j][i].clear();
            //lastshowermuonTrigtimeVec[j][i].clear();
            //muonSpecVec[j][i].clear();
            lastshowermuonTrigtimeNoRed[j][i].SetSec(0);
            lastshowermuonTrigtimeNoRed[j][i].SetNanoSec(0);
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

    
    time2Allmuon=new TH1F("time2Allmuon","time2Allmuon",100000,0.,100.);
	ntupleSvc()->attach("FILE1/lidj",time2Allmuon);
    muonSpec=new TH1F("muonSpec","muonSpec",MAXNUM,energySlice);
    ntupleSvc()->attach("FILE1/lidj",muonSpec);
    muonSpecNoRed=new TH1F("muonSpecNoRed","muonSpecNoRed",MAXNUM,energySlice);
	ntupleSvc()->attach("FILE1/lidj",muonSpecNoRed);
    for( int i=0 ; i<4 ; i++ )
    {
        histName="t2nextEvtAD";
        histName+=i+1;
        t2nextEvt[i]=new TH1F(histName,histName,90,2.e-4,2.e-3);
	    ntupleSvc()->attach("FILE1/lidj",t2nextEvt[i]);
    }
    
    time2Allmuon4Li=new TH1F("time2Allmuon4Li","time2Allmuon4Li",100000,0.,100.);
	ntupleSvc()->attach("FILE1/lidj",time2Allmuon4Li);
	if( saveSingle )
	{
		SingleTree = ntupleSvc()->bookTree("FILE2/Tree/SingleTree","SingleTree");
		SingleTree->Branch("energy",&energy,"energy/F");
		SingleTree->Branch("x",&x,"x/F");
		SingleTree->Branch("y",&y,"y/F");
		SingleTree->Branch("z",&z,"z/F");
		SingleTree->Branch("det",&det,"det/I");
		//SingleTree->Branch("t2muon",t2muon,"t2muon[7]/F");
		SingleTree->Branch("t2lastshowermuon",&t2lastshowermuon,"t2lastshowermuon/F");
        SingleTree->Branch("T2lastMuon",T2lastMuon,"T2lastMuon[3564]/F");
	}

	LiTree = ntupleSvc()->bookTree("FILE2/Tree/LiTree","LiTree");
	LiTree->Branch("det",&det_l,"det_l/I");
	LiTree->Branch("energy",energy_l,"energy_l[2]/F");
	LiTree->Branch("x",x_l,"x_l[2]/F");
	LiTree->Branch("y",y_l,"y_l[2]/F");
	LiTree->Branch("z",z_l,"z_l[2]/F");
	LiTree->Branch("timeInterval",&timeInterval,"timeInterval/F");
	//LiTree->Branch("promptT2Muon",promptT2Muon,"promptT2Muon[7]/F");
	LiTree->Branch("t2lastshowermuon",&t2lastshowermuon,"t2lastshowermuon/F");

	//MutilTree = ntupleSvc()->bookTree("","");
	//MutilTree->Branch("",&,"");


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

        double AdMuonEnergy=0.;//CurEvent->energy();
        AdMuonEnergy=CurEvent->energy();

        /*
        */
        muonSpecNoRed->Fill(AdMuonEnergy);
        for( int i=0 ; i<MAXNUM ; i++ )
        {
            if( AdMuonEnergy>energySlice[i]&& AdMuonEnergy<energySlice[i+1] )
            {
                tAnde tande={CurEvent->m_trigTime,CurEvent->energy()};
                //lastshowermuonTrigtimeVec[i][CurEvent->m_det-1].push_back(CurEvent->m_trigTime);
                //muonSpecVec[i][CurEvent->m_det-1].push_back(CurEvent->energy());
                tAndeVec[i][CurEvent->m_det-1].push_back(tande);
                lastshowermuonTrigtimeNoRed[i][CurEvent->m_det-1].SetSec(CurEvent->m_trigTime.GetSec());
                lastshowermuonTrigtimeNoRed[i][CurEvent->m_det-1].SetNanoSec(CurEvent->m_trigTime.GetNanoSec());
                break;
            }
        }
        
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
        for( int i=0 ; i<MAXNUM ; i++ )
        {
            if( !tAndeVec[i][CurEvent->m_det-1].empty() )
            {
                vector<tAnde>::iterator it=tAndeVec[i][CurEvent->m_det-1].end();
                it--;
                bool lastAdmuon=true;
                for(; it>=tAndeVec[i][CurEvent->m_det-1].begin();it--) 
                {
                    if( (CurEvent->m_trigTime-(*it).time)>1.e-5 &&(CurEvent->m_trigTime-(*it).time)<2.e-4)
                    {
                        if( lastAdmuon )
                        {
                            lastshowermuonTrigtime[i][CurEvent->m_det-1].SetSec((*it).time.GetSec());
                            lastshowermuonTrigtime[i][CurEvent->m_det-1].SetNanoSec((*it).time.GetNanoSec());
                            lastAdmuon=false;
                        }
                        lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].SetSec((*it).time.GetSec());
                        lastshowermuonTrigtimeTmp[i][CurEvent->m_det-1].SetNanoSec((*it).time.GetNanoSec());
                        muonSpec->Fill((*it).energy);
                        it=tAndeVec[i][CurEvent->m_det-1].erase(it);
                    } else if( (CurEvent->m_trigTime-(*it).time)>=2.e-4 )
                    {
                        it++;
                        tAndeVec[i][CurEvent->m_det-1].erase(tAndeVec[i][CurEvent->m_det-1].begin(),it); 
                        break;
                    } else
                    {
                        //
                    }
                }
            }
        }
        
    }


	int vetotag=muonVeto_l->veto(CurEvent);

	if( vetotag==1 )
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
    
	return true;
}

bool IsotopesAna::FillLi(vector<PhyEvent*> EvtGroup)
{

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
        return true;
    }
	if( saveSingle )
	{
		det=Evt->m_det;
		x=Evt->m_x;
		y=Evt->m_y;
		z=Evt->m_z;
		//CalTime2Muon(Evt);
		for(int i=0;i<MAXNUM*2+4;i++) T2lastMuon[i]=time2MuonBuf[Evt->m_det-1][0][i];
        t2lastshowermuon=time2MuonBuf[Evt->m_det-1][0][1];
		SingleTree->Fill();
	}
	return true;
}

bool IsotopesAna::CalTime2Muon(PhyEvent* event)
{
    //can not be used in dump().must only be used for current event,because these last*muonTrigtimes are latest muons!!!
    time2MuonVec.clear();
	for(int i=0;i<MAXNUM*2+4;i++) time2Muon[i]=10.e6;
	//pre ADMuon
    if( lastAdMuonTrigtime[event->m_det-1].GetSec()!=0. )
    {
        time2Muon[0]=event->m_trigTime-lastAdMuonTrigtime[event->m_det-1];
    }
	//pre ADshower muon
    if( lastShowerMuonTrigtime[event->m_det-1].GetSec()!=0. )
    {
        time2Muon[1]=event->m_trigTime - lastShowerMuonTrigtime[event->m_det-1];
    }
	//pre Iwp muon
    if( lastIwpMuonTrigtime.GetSec()!=0. )
    {
        time2Muon[2]=event->m_trigTime - lastIwpMuonTrigtime;
    }
	//pre Owp muon
    if( lastOwpMuonTrigtime.GetSec()!=0. )
    {
        time2Muon[3]=event->m_trigTime - lastOwpMuonTrigtime;
    }
    //for B12/N12
    for( int i=0 ; i<MAXNUM ; i++ )
    {
        if( lastshowermuonTrigtime[i][event->m_det-1].GetSec()!=0. )
        {
            time2Muon[i+4]=event->m_trigTime - lastshowermuonTrigtime[i][event->m_det-1];
        }
        if( lastshowermuonTrigtimeNoRed[i][event->m_det-1].GetSec()!=0. )
        {
            time2Muon[i+4+MAXNUM]=event->m_trigTime - lastshowermuonTrigtimeNoRed[i][event->m_det-1];
        }
    }
    

    for( int i=0 ; i<MAXNUM*2+4 ; i++ )
    {
        time2MuonVec.push_back(time2Muon[i]);
    }
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
