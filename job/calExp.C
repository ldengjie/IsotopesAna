{
    double a[3]={20.5,15.5,1.};
    double a2[3]={200.,150.,20.};
    double d[7]={0.0159,0.0291,0.182,0.171,0.257,1.21,1.11};
    string cs[7]={"N12","B12","C9","He8","Li9","Li8","B8"};
    for( int j=0 ; j<3 ; j++ )
    {
        cout<<"EH"<<j+1<<"  : "<<endl;
        for( int i=0 ; i<7 ; i++ )
        {
            double b=1/d[i];
            double c=a[j]/b*(-exp(-100.e-3*b)+exp(-2.e-3*b)+exp(-200.e-3*b)-exp(-102.e-3*b));
            double e1=a[j]/b*(1-exp(-1.e-3*b))*exp(-b*100.e-3);
            double e2=a2[j]/b*(exp(200.e-6*b)-exp(-200.e-6*b));
            cout<<cs[i]<<"  : "<<c<<" | "<<e1+e2<<endl;
        }
    }
    
    
}
