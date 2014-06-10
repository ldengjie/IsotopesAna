#include  <iostream>
#include  <TChain.h>
#include  <TROOT.h>
#include  <string>
using namespace std;
int anaTree(int siteNum,string dataVer)
{
	gROOT->ProcessLine(".L SingleTree.C+");
	TChain chain("Tree/SingleTree");
	string site;
    if( siteNum==1 )
	{   
	    site="EH1";
	} else if(siteNum==2)
	{   
	    site="EH2";
	}else if (siteNum==3)
	{   
	    site="EH3";
	}else
	{   
	    site="EH1";
	} 

	string chainFile;
	
    chainFile="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/";
    chainFile+=dataVer;
    chainFile+="/";
    chainFile+=site;
    chainFile+="/*IsotopesAna.root";
    std::cout<<"chainFile  : "<<chainFile<<endl;
    chain.Add(chainFile.c_str());
    std::cout<<dataVer<<" : "<<chain.GetEntries()<<endl;
    if( dataVer=="P12E" )
    {
        chainFile="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/";
        chainFile+="P12A";
        chainFile+="/";
        chainFile+=site;
        chainFile+="/*IsotopesAna.root";
        chain.Add(chainFile.c_str());
        std::cout<<dataVer<<"+P12A : "<<chain.GetEntries()<<endl;
    }
    string  siteAndDataVer=site+dataVer+"B12";
    std::cout<<"site And DataVer  : "<<siteAndDataVer<<endl;
    std::cout<<"lidj total single number  : "<<chain.GetEntries()<<endl;

    chain.Process("SingleTree",siteAndDataVer.c_str());
    //siteAndDataVer=site+dataVer+"Li8";
    //chain.Process("SingleTree",siteAndDataVer.c_str());
    //siteAndDataVer=site+dataVer+"C99";//C9
    //chain.Process("SingleTree",siteAndDataVer.c_str());
    siteAndDataVer=site+dataVer+"N12";//C9
    chain.Process("SingleTree",siteAndDataVer.c_str());
	return 0;
}
    int GenIsoSpec(string dataVer,int sitenum=0)
//int main(int argc, char *argv[])
{
    //string dataVer=argv[1];
    //int sitenum=atoi(argv[2]);
    if( !(dataVer=="P13A"||dataVer=="P12E"||dataVer=="P14A") )
    {
        std::cout<<"  The dataVerion is wrong ,please check! "<<endl;
    }
    if( sitenum==0 )
    {
        anaTree(1,dataVer);
        anaTree(2,dataVer);
        anaTree(3,dataVer);
    }
    if( sitenum==1 )
    {
        anaTree(1,dataVer);
    }
    if( sitenum==2 )
    {
        anaTree(2,dataVer);
    }
    if( sitenum==3 )
    {
        anaTree(3,dataVer);
    }
    return 0;
}
