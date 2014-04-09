{
	gROOT->ProcessLine(".L liuyb.C+");
	TChain chain("liuyb/Singles");
	string chainFile;
	
    //chainFile="/publicfs/dyb/data/userdata/liuyb/LAF/SinglesAlg/run/P13A/EH1/data/P13A/singles_*.root";
    chainFile="/publicfs/dyb/data/userdata/liuyb/LAF/SinglesAlg/run/P13A/EH1/data/P13A/singles_*.root";
    //chainFile="/publicfs/dyb/data/userdata/liuyb/LAF/SinglesAlg/run/P13A/EH1/data/P13A/singles_37661.root";
    chain.Add(chainFile.c_str());
    std::cout<<"liuyb total number  : "<<chain.GetEntries()<<endl;
    chain.Process("liuyb");
	return 0;
}
