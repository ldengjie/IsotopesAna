#include  <iostream>
#include  "TH1F.h"
#include  "TTree.h"
#include  "TMath.h"
#include  <string>
#include  "MuonVeto/MuonVeto.h"
#include  "LafKernel/PhyEvent.h"
#include  <vector>
#include  "LafKernel/OptionParser.h"
#include  "LafKernel/AlgBase.h"
#include  "LafKernel/DataBuffer.h"
#include  "LafKernel/GlobalVar.h"
#include  "LafKernel/LafLog.h"
#include  "LiveTimeSvc/LiveTimeSvc.h"
#include    "LafKernel/PhyEvent/CalibReadout.h"
using namespace std;  

class IsotopesAna : public AlgBase
{
  public:
	IsotopesAna(const std::string& name);
	virtual ~IsotopesAna(){}

	virtual bool initialize();
	virtual bool execute();
	virtual bool finalize();
  private:
	bool FillLi(vector<PhyEvent*> EvtGroup);
	bool FillSingle(PhyEvent* Evt);
	bool CalTime2Muon(PhyEvent* event);
	bool printEvt(PhyEvent* CurEvent);
    void dump(int i_det,bool IsFinal,PhyEvent* CurEvent);

    int ADMuonNum[7];
	float time2Muon[16];
	TString histName;
	//int Ebins;
	int det;
	float t2muon[16];
	
	bool saveSingle;
	TTree* SingleTree;
	float x;
	float y;
	float z;
	float energy;
	float T2lastMuon[16];

	TTree* LiTree;
	//[0]for IBD Prompt singal,[1]for IBD delayed singal
	int    det_l;
	float  energy_l[2];
	float  x_l[2];
	float  y_l[2];
	float  z_l[2];
	float timeInterval; //us
	float promptT2Muon[16];
    float t2lastshowermuon;
	float delayedT2Muon[16];

    TTimeStamp lastOwpMuonTrigtime;
    TTimeStamp lastIwpMuonTrigtime;
    TTimeStamp lastRpcMuonTrigtime;
    TTimeStamp lastAdMuonTrigtime[4];
    TTimeStamp lastShowerMuonTrigtime[4];

	TTimeStamp lastshowermuonTrigtimeNoRed[6][4];
    TH1F* time2lastshowermuonNoRed[6];
	TH1F* showermuonNumNoRed[6];

	TTimeStamp lastshowermuonTrigtime[6][4];
	TTimeStamp lastshowermuonTrigtimeTmp[6][4];
	vector<TTimeStamp> lastshowermuonTrigtimeVec[6][4];
    TH1F* time2lastshowermuon[6];
	TH1F* showermuonNum[6];
    TH1F* time2lastshowermuon4Li[6];
    TH1F* time2Allmuon;
    TH1F* t2nextEvt[4];
    TH1F* time2Allmuon4Li;

	LiveTimeSvc* liveTimeSvc;
	PhyEventBuf* EvtBuffer;
	PhyEvent* CurEvent;
	vector<PhyEvent*> AdEvtBuf[4];
    vector<double> time2MuonVec;
    vector< vector<double> > time2MuonBuf[4];
	MuonVeto* muonVeto_l;
	
	double ELow;//PromptEnergyLow
	double EHigh;//PromptEnergyHigh
	double promptELow4Li;//PromptEnergyLow
	double promptEHigh4Li;//PromptEnergyHigh
	double delayedELow4Li;//DelayedEnergyLow
	double delayedEHigh4Li;//DelayedEnergyHigh
	double AdMuonELow;//AdMuonELow
	double AdMuonEHigh;//AdMuonEHigh

	double LiIntervalMin;//LiIntervalMin
	double LiIntervalMax;//LiIntervalMax
	double Time2LastBufEvent;//Time2LastBufEvent
	double DelayedTrigTime2AdMuon4Li;
	double DelayedTrigTime2IWpMuon4Li;
	double DelayedTrigTime2OWpMuon4Li;
	double DelayedTrigTime2AdShowerMuon4Li;
	//double Time2PreAdEvent;//Time2PreAdEvent
	//double Time2NextAdEvent;//Time2NextAdEvent
	//double Time2PreAdEvent_l;
	//double Time2NextAdEvent_l;
};
