#include  <iostream>
#include <time.h>//get system time
#include  <TChain.h>
#include  <TROOT.h>
#include  <string>
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
    chain.Add(chainFile.c_str());
    std::cout<<"P12E : "<<chain.GetEntries()<<endl;
    if( dataVer=="P12E" )
    {
        chainFile="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/";
        chainFile+="P12A";
        chainFile+="/";
        chainFile+=site;
        chainFile+="/*IsotopesAna.root";
        chain.Add(chainFile.c_str());
    }

    std::cout<<"P12E+P12A : "<<chain.GetEntries()<<endl;
    std::cout<<"lidj total single number  : "<<chain.GetEntries()<<endl;
    string nameStr;
    string isoName[4]={"B12","C99","N12","Li8"};
    double exRate[10]={0.05,0.1,0.5,1.,1.5,2.,2.5,3.,3.5,4.};
    for( int i=0 ; i<3 ; i++ )
    {
       for( int j=0 ; j<10 ; j++ )
       {
           nameStr=Form("%s%s%s%0.2f",site.c_str(),dataVer.c_str(),isoName[i].c_str(),exRate[j]);
           std::cout<<"nameStr  : "<<nameStr<<endl;
           //chain.Process("SingleTree",nameStr.c_str());
       }
       
    }
    
	return 0;
}
int GenIsoSpec(string dataVer,int sitenum=0)
{
    time_t t0 = time(0); 
    char tmp0[64]; 
    strftime( tmp0, sizeof(tmp0), "%Y/%m/%d %X %A %j %z",localtime(&t0) ); 
    std::cout<<"begin  at "<<tmp0<<endl;
    //string dataVer="P13A";
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
    time_t t = time(0); 
    char tmp[64]; 
    strftime( tmp, sizeof(tmp), "%Y/%m/%d %X %A %j %z",localtime(&t) ); 
    std::cout<<"all is done at "<<tmp<<endl;
    return 0;
}
