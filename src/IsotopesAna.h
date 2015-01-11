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
	bool FillSingle(PhyEvent* Evt);
	bool CalTime2Muon(PhyEvent* event);
	bool printEvt(PhyEvent* CurEvent);
    void dump(int i_det,bool IsFinal,PhyEvent* CurEvent);

    int ADMuonNum[7];
	double time2Muon[64];
	TString histName;
	//int Ebins;
	int det;
	double t2muon[64];
	
	bool saveSingle;
	TTree* SingleTree;
	float x;
	float y;
	float z;
	float energy;
	double T2lastMuon[64];
    double triggerTime;

    double t2lastshowermuon;
    int i4num1[4];
    int i4num2[4];

    TTimeStamp lastOwpMuonTrigtime;
    TTimeStamp lastIwpMuonTrigtime;
    TTimeStamp lastRpcMuonTrigtime;
    TTimeStamp lastAdMuonTrigtime[4];
    TTimeStamp lastShowerMuonTrigtime[4];

	TTimeStamp lastshowermuonTrigtimeNoRed[6][4];
    TH1F* time2lastshowermuonNoRed[6];
	TH1F* showermuonNumNoRed[6];
    bool statMuonTimeInterval;
	TH1F* muonTimeIntervalNoRed[6];
	TH1F* muonTimeInterval[6];
	TH1F* muonEnergy[6];
	TH1F* muonEnergyI[4][6];
	TH1F* muonEnergyNoRed[6];


	TTimeStamp lastshowermuonTrigtime[6][4];
	TTimeStamp lastlastshowermuonTrigtime[6][4];
	TTimeStamp lastshowermuonTrigtimeTmp[6][4];
	vector<TTimeStamp> lastshowermuonTrigtimeVec[6][4];
	vector<float> muonEnergyVec[6][4];

	TTimeStamp lastshowermuonTrigtimeI[4][6][4];
        vector<double> adMuonTriggerTimeBuf[4][6][4];
        vector<double> adMuonEnergyBuf[4][6][4];
        PhyEvent* finalTestADMuon[4][6][4];
        double nextImuonTriggerTime[4][6][4];

    TH1F* time2lastshowermuon[6];
	TH1F* showermuonNum[6];
    TH1F* time2lastshowermuonI[4][6];
	TH1F* showermuonNumI[4][6];
    TH1F* time2Allmuon;
    TH1F* t2nextEvt[4];

	LiveTimeSvc* liveTimeSvc;
	PhyEventBuf* EvtBuffer;
	PhyEvent* CurEvent;
	vector<PhyEvent*> AdEvtBuf[4];
    vector<double> time2MuonVec;
    vector< vector<double> > time2MuonBuf[4];
	MuonVeto* muonVeto_l;
	
	float ELow;//PromptEnergyLow
	float EHigh;//PromptEnergyHigh
	float AdMuonELow;//AdMuonELow
	float AdMuonEHigh;//AdMuonEHigh

	double Time2LastBufEvent;//Time2LastBufEvent
};
