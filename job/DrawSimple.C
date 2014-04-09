{
    //int EventNum=chain->GetEntries();
    //double x,y,z;
    //chain->SetBranchAddress("x",x);
    //for( int i=0 ; i<EventNum ; i++ )
    //{
    //GetEntry(i);
    //}
    string chainFile;
    chainFile="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P14A/EH3/run2*IsotopesAna.root";
    TChain* chain=new TChain("Tree/SingleTree");
    chain->Add(chainFile.c_str());
    chain->Draw("z:sqrt(x*x+y*y)","","COLZ"); 
    std::cout<<"total  : "<<chain->GetEntries()<<endl;
    std::cout<<"z>0    : "<<chain->GetEntries("z>0")<<endl;
    std::cout<<"z<0    : "<<chain->GetEntries("z<0")<<endl;

}
