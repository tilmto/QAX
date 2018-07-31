#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#define infinity 10000000

using namespace std;

///////////////  Beginning: Original Hardware Based on Swap  ///////////////////

class HardwareA
{
protected:
    int qubitNum;

    int edgeNum;

    bool isUniDirection;

    vector<vector<bool>> archMatrix;

    vector<vector<int>> distMatrix;

    vector<vector<int>> routeMatrix;

    vector<int> outdeg;

    vector<int> mapArray;

public:
    HardwareA(string hwname,bool isUniDirection);

    int GetQNum();

    int GetENum();

    void GetArch(string hwname);

    void PrintArchMatrix();

    void Floyd();

    void VerifyRouteMatrix();

    void PrintRouteMatrix();

    void PrintPath(int i,int j);

    void PrintMap();

    void InitMap(vector<vector<int>> seq);

    int Alloc(vector<vector<int>> seq);

};


HardwareA::HardwareA(string hwname,bool isUniDirection=true)
{
    this->isUniDirection=isUniDirection;

    GetArch(hwname);

    PrintArchMatrix();

    Floyd();

    cout << "Physical qubits number: " << qubitNum << endl;
    cout << "Edge number: " << edgeNum << endl;
}


void HardwareA::GetArch(string hwname)
{
    int adjIndex,i;

    ifstream is(hwname,ios::in);
    if(!is)
    {
        cout << "Cannot Open Hardware File." << endl;
        exit(1);
    }

    qubitNum=0;
    edgeNum=0;

    while(!is.eof())
    {
        is>>adjIndex;
        if(adjIndex == -1)
            qubitNum++;
    }
    qubitNum--;

    for(i=0; i<qubitNum; i++)
    {
        archMatrix.push_back(vector<bool>(qubitNum,false));
        distMatrix.push_back(vector<int>(qubitNum));
        routeMatrix.push_back(vector<int>(qubitNum));
        outdeg.push_back(0);
    }

    mapArray.resize(qubitNum);

    i=0;
    is.clear();
    is.seekg(0,ios::beg);

    while(i<qubitNum && !is.eof())
    {
        is>>adjIndex;
        if(adjIndex==-1)
        {
            archMatrix[i][i]=true;
            i++;
        }

        else
        {
            archMatrix[i][adjIndex]=true;
            outdeg[i]++;
            edgeNum++;
        }
    }

    is.close();
}


void HardwareA::PrintArchMatrix()
{
    cout << "Architecture Matrix:" << endl;
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
        {
            cout << archMatrix[i][j] << " ";
            if(j==qubitNum-1)
                cout << endl;
        }
}


void HardwareA::Floyd()
{
    int i,j,k;
    for(i=0; i<qubitNum; i++)
        for(j=0; j<qubitNum; j++)
        {
            if(!archMatrix[i][j] && !archMatrix[j][i])
            {
                distMatrix[i][j]=infinity;
                routeMatrix[i][j]=-1;
            }

            else
            {
                distMatrix[i][j]=1;
                routeMatrix[i][j]=j;
            }
        }

    for(k=0; k<qubitNum; k++)
        for(i=0; i<qubitNum; i++)
            for(j=0; j<qubitNum; j++)
                if(distMatrix[i][j]>distMatrix[i][k]+distMatrix[k][j])
                {
                    distMatrix[i][j]=distMatrix[i][k]+distMatrix[k][j];
                    routeMatrix[i][j]=routeMatrix[i][k];
                }

    VerifyRouteMatrix();

    PrintRouteMatrix();
}



void HardwareA::VerifyRouteMatrix()
{
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
            if(routeMatrix[i][j]==-1)
            {
                cout << "Not fully connected architecture." << endl;
                exit(1);
            }
}


void HardwareA::PrintRouteMatrix()
{
    cout << "Route Matrix:" << endl;
    for(int i=0; i<qubitNum; i++)
        for(int j=0; j<qubitNum; j++)
        {
            cout << routeMatrix[i][j] << " ";
            if(j==qubitNum-1)
                cout << endl;
        }
}


void HardwareA::PrintPath(int i,int j)
{
    int next=routeMatrix[i][j];
    if(next==-1)
        cout << "No Path between " << i << " and "<< j << endl;
    else
    {
        cout << "Path from " << i << " to " << j << ": " << i << " ";
        while(next!=j)
        {
            cout << next << " ";
            next=routeMatrix[next][j];
        }
        cout << j << endl;
    }
}

int HardwareA::GetQNum()
{
    return qubitNum;
}


int HardwareA::GetENum()
{
    return edgeNum;
}


void HardwareA::InitMap(vector<vector<int>> seq)
{
    int i;
    unsigned int j;
    vector<int> freq(qubitNum,0);
    vector<int> sortFreq(1,0);
    vector<int> sortOutDeg(1,0);

    for(j=0; j<seq.size(); j++)
        freq[seq[j][0]]++;

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortFreq.size(); j++)
        {
            if(freq[i]>freq[sortFreq[j]])
            {
                sortFreq.insert(sortFreq.begin()+j,i);
                break;
            }

            if(j==sortFreq.size()-1)
            {
                sortFreq.push_back(i);
                break;
            }
        }

    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortOutDeg.size(); j++)
        {
            if(outdeg[i]>outdeg[sortOutDeg[j]])
            {
                sortOutDeg.insert(sortOutDeg.begin()+j,i);
                break;
            }

            if(j==sortOutDeg.size()-1)
            {
                sortOutDeg.push_back(i);
                break;
            }
        }

    for(i=0; i<qubitNum; i++)
        mapArray[sortOutDeg[i]]=sortFreq[i];

    /*
    cout << "Initial Mapping:" << endl;
    PrintMap();
    */
}


void HardwareA::PrintMap()
{
    int i;
    cout << "Physical qubits: ";
    for(i=0; i<qubitNum; i++)
        cout << i << " ";
    cout << endl;
    cout << "Pseudo   qubits: ";
    for(i=0; i<qubitNum; i++)
        cout << mapArray[i] << " ";
    cout << endl;
}


int HardwareA::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    int j,temp,current,next,dest,cost=0;

    for(i=0; i<seq.size(); i++)
    {
        for(j=0; j<qubitNum; j++)
        {
            if(mapArray[j]==seq[i][1])
                current=j;

            if(mapArray[j]==seq[i][0])
                dest=j;
        }

        next=routeMatrix[current][dest];

        while(next!=dest)
        {
            temp=mapArray[current];
            mapArray[current]=mapArray[next];
            mapArray[next]=temp;

            cost=cost+7;

            current=next;
            next=routeMatrix[current][dest];
        }

        if(archMatrix[current][next])
            cost++;
        else
            cost=cost+5;

        /*
        cout << "After the " << i+1 << "th operation:" << endl;
        PrintMap();
        cout << endl;
        */
    }

    return cost;
}

///////////////  Ending: Original Hardware Based on Swap  ///////////////////

int frac(int n);

////////////////////  Beginning: Hardware with bridge  //////////////////////

class HardwareB:public HardwareA
{
public:
    HardwareB(string hwname,bool isUniDirection);

    void InitMap(vector<vector<int>> seq,vector<int> sortOutDeg);

    int Alloc(vector<vector<int>> seq);

    void SubAlloc(vector<vector<int>> worklist,vector<int> mapArray,vector<int>& minmap,int& mincost);
};

HardwareB::HardwareB(string hwname,bool isUniDirection=true):HardwareA(hwname,isUniDirection) {}

void HardwareB::InitMap(vector<vector<int>> seq,vector<int> sortOutDeg)
{
    int i;
    unsigned int j;
    vector<int> freq(qubitNum,0);
    vector<int> sortFreq(1,0);
    //vector<int> sortOutDeg(1,0);

    for(j=0; j<seq.size(); j++)
    {
        freq[seq[j][1]]++;
    }


    for(i=1; i<qubitNum; i++)
        for(j=0; j<sortFreq.size(); j++)
        {
            if(freq[i]>freq[sortFreq[j]])
            {
                sortFreq.insert(sortFreq.begin()+j,i);
                break;
            }

            if(j==sortFreq.size()-1)
            {
                sortFreq.push_back(i);
                break;
            }
        }
/*
    sortOutDeg.clear();
    sortOutDeg.push_back(4);
    sortOutDeg.push_back(13);
    sortOutDeg.push_back(5);
    sortOutDeg.push_back(12);
    sortOutDeg.push_back(14);
    sortOutDeg.push_back(6);
    sortOutDeg.push_back(11);
    sortOutDeg.push_back(3);
    sortOutDeg.push_back(10);
    sortOutDeg.push_back(15);
    sortOutDeg.push_back(7);
    sortOutDeg.push_back(2);
    sortOutDeg.push_back(0);
    sortOutDeg.push_back(9);
    sortOutDeg.push_back(8);
    sortOutDeg.push_back(1);

    /*
        for(i=1; i<qubitNum; i++)
            for(j=0; j<sortOutDeg.size(); j++)
            {
                if(outdeg[i]>outdeg[sortOutDeg[j]])
                {
                    sortOutDeg.insert(sortOutDeg.begin()+j,i);
                    break;
                }

                if(j==sortOutDeg.size()-1)
                {
                    sortOutDeg.push_back(i);
                    break;
                }
            }
    */

    for(i=0; i<qubitNum; i++)
        mapArray[sortOutDeg[i]]=sortFreq[i];
/*
    int current,dest,next,temp,cnt=0;
    i=0;
    while(1)
    {
        if(seq[i][0]!=-1)
        {
            for(j=0; j<qubitNum; j++)
            {
                if(mapArray[j]==seq[i][0])
                    current=j;

                if(mapArray[j]==seq[i][1])
                    dest=j;
            }

            if(routeMatrix[current][dest]!=dest)
            {
                next=routeMatrix[current][dest];

                while(next!=dest)
                {
                    temp=mapArray[current];
                    mapArray[current]=mapArray[next];
                    mapArray[next]=temp;

                    current=next;
                    next=routeMatrix[current][dest];
                }

                cnt++;
            }


                        if(routeMatrix[current][dest]!=dest)
                        {
                            for(j=0;j<qubitNum;j++)
                                if(archMatrix[j][dest])
                                    break;

                            if(j>=qubitNum)
                                j=routeMatrix[dest][current];

                            temp=mapArray[current];
                            mapArray[current]=mapArray[j];
                            mapArray[j]=temp;

                            cnt++;
                        }

        }

        if(cnt>=4 || i>=seq.size()-1)
            break;

        i++;
    }
*/
    /*
    cout << "Initial Mapping:" << endl;
    PrintMap();
    */
}


int HardwareB::Alloc(vector<vector<int>> seq)
{
    unsigned int i,j;
    int m,n,seqLen,permuteNum,mincost,totalcost=0;
    vector<vector<int>> worklist;
    vector<int> minmap;
    vector<int> temp;

    for(i=0; i<seq.size(); )
    {
        for(j=0; j<worklist.size(); j++)
        {
            if(seq[i][0]==worklist[j][0] || seq[i][0]==worklist[j][1] || seq[i][1]==worklist[j][0] || seq[i][1]==worklist[j][1])
                break;
        }

        if(j>=worklist.size())
        {
            worklist.push_back(seq[i]);
            i++;
        }

        else
        {
            mincost=infinity;
            seqLen=worklist.size();

            if(seqLen==1)
                SubAlloc(worklist,mapArray,minmap,mincost);

            else
            {
                permuteNum=frac(seqLen);
                m=0;
                while(m<permuteNum)
                {
                    for(n=seqLen-1; n>0; n--)
                    {
                        temp=worklist[n];
                        worklist[n]=worklist[n-1];
                        worklist[n-1]=temp;
                        m++;
                        SubAlloc(worklist,mapArray,minmap,mincost);
                    }

                    temp=worklist[seqLen-1];
                    worklist[seqLen-1]=worklist[seqLen-2];
                    worklist[seqLen-2]=temp;
                    m++;
                    SubAlloc(worklist,mapArray,minmap,mincost);

                    for(n=0; n<seqLen-1; n++)
                    {
                        temp=worklist[n];
                        worklist[n]=worklist[n+1];
                        worklist[n+1]=temp;
                        m++;
                        SubAlloc(worklist,mapArray,minmap,mincost);
                    }

                    temp=worklist[0];
                    worklist[0]=worklist[1];
                    worklist[1]=temp;
                    m++;
                    SubAlloc(worklist,mapArray,minmap,mincost);
                }
            }

            mapArray=minmap;
            totalcost=totalcost+mincost;
            worklist.clear();

        }

        /*
        cout << "After the " << i+1 << "th operation:" << endl;
        PrintMap();
        cout << endl;
        */
    }

    return totalcost;
}


void HardwareB::SubAlloc(vector<vector<int>> worklist,vector<int> mapArray,vector<int>& minmap,int& mincost)
{
    unsigned int i;
    int j,current,next,dest,temp,cost=0;

    for(i=0; i<worklist.size(); i++)
    {
        for(j=0; j<qubitNum; j++)
        {
            if(mapArray[j]==worklist[i][0])
                current=j;

            if(mapArray[j]==worklist[i][1])
                dest=j;
        }

        next=routeMatrix[current][dest];

        if(next==dest)
        {
            if(archMatrix[current][next])
                cost++;
            else
                cost=cost+5;
        }

        else
        {
            while(routeMatrix[next][dest]!=dest)
            {
                temp=mapArray[current];
                mapArray[current]=mapArray[next];
                mapArray[next]=temp;

                cost=cost+7;

                current=next;
                next=routeMatrix[current][dest];
            }

            if(archMatrix[current][next] && archMatrix[next][dest])
                cost=cost+4;
            else if((!archMatrix[current][next] && !archMatrix[next][dest]) || (archMatrix[current][next] && !archMatrix[next][dest]))
                cost=cost+10;
            else
            {
                temp=mapArray[current];
                mapArray[current]=mapArray[next];
                mapArray[next]=temp;
                cost=cost+8;
            }
        }
    }

    if(cost<mincost)
    {
        mincost=cost;
        minmap=mapArray;
    }
}


///////////////  Ending: Hardware with bridge  /////////////////////

/*
////////  Beginning: Hardware with bridge and fixed qubits  ////////

class HardwareC:public HardwareB
{
protected:
    vector<int> fixedQubit;

public:
    HardwareC(string hwname,bool isUniDirection);

    int Alloc(vector<vector<int>> seq);
};

HardwareC::HardwareC(string hwname,bool isUniDirection=true):HardwareB(hwname,isUniDirection)
{
    fixedQubit.push_back(6);
    fixedQubit.push_back(8);
}


int HardwareC::Alloc(vector<vector<int>> seq)
{
    unsigned int i;
    int j,temp,current,next,dest,cost=0;

    for(i=0; i<seq.size(); i++)
    {
        for(j=0; j<qubitNum; j++)
        {
            if(mapArray[j]==seq[i][1])
                current=j;

            if(mapArray[j]==seq[i][0])
                dest=j;
        }

        if(outdeg[current]>outdeg[dest])
        {
            temp=current;
            current=dest;
            dest=temp;
        }

        next=routeMatrix[current][dest];

        if(next==dest)
            cost++;
        else
        {
            while(routeMatrix[next][dest]!=dest)
            {
                if(next!=6 && next!=8)
                {
                    temp=mapArray[current];
                    mapArray[current]=mapArray[next];
                    mapArray[next]=temp;
                    cost=cost+7;
                    current=next;
                    next=routeMatrix[current][dest];
                }

                else
                {
                    for(j=0; j<qubitNum; j++)
                        if(routeMatrix[current][j]==j && routeMatrix[j][dest]!=current && j!=6 && j!=8)
                        {
                            next=j;
                            break;
                        }
                }
            }

            cost=cost+4;
        }

        cout << "After the " << i+1 << "th operation:" << endl;
        PrintMap();
        cout << endl;
    }

    return cost;
}

//////////  Ending: Hardware with bridge and fixed qubits  ///////////
*/

void RandSeqGen(vector<vector<int>> &seq,int qubitNum,int seqLen);

int GetSeq(vector<vector<int>> &seq,string fname);

void PrintSeq(vector<vector<int>> seq);

int GetSeqList(vector<string> &fileList, string directory);

int main()
{
    int costA,costB,scount,fcount;
    clock_t starttime,endtime;

    HardwareA archA("ibmqx5");
    HardwareB archB("ibmqx5");

    vector<string> fileList;
    vector<vector<int>> seq;




    scount=GetSeq(seq,"seq/seq_ground_state_estimation_10.qasm");

    archA.InitMap(seq);
    costA=archA.Alloc(seq)+scount;

//    vector<int> sortOutDeg={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    vector<int> sortOutDeg={4,13,5,12,14,6,11,3,10,15,7,2,0,9,8,1};

    vector<int> minsort;
    int mincost=infinity;
    int i,j,temp;
    int cnt=frac(6);

    i=0;
    while(i<cnt)
    {
        for(j=sortOutDeg.size()-1;j>0;j--)
        {
            temp=sortOutDeg[j];
            sortOutDeg[j]=sortOutDeg[j-1];
            sortOutDeg[j-1]=temp;
            i++;

            archB.InitMap(seq,sortOutDeg);
            costB=archB.Alloc(seq);
            if(costB<mincost)
            {
                mincost=costB;
                minsort=sortOutDeg;
            }
        }

        temp=sortOutDeg[sortOutDeg.size()-1];
        sortOutDeg[sortOutDeg.size()-1]=sortOutDeg[sortOutDeg.size()-2];
        sortOutDeg[sortOutDeg.size()-2]=temp;
        i++;

        archB.InitMap(seq,sortOutDeg);
        costB=archB.Alloc(seq);
        if(costB<mincost)
        {
            mincost=costB;
            minsort=sortOutDeg;
        }

        for(j=0;j<sortOutDeg.size()-1;j++)
        {
            temp=sortOutDeg[j];
            sortOutDeg[j]=sortOutDeg[j+1];
            sortOutDeg[j+1]=temp;
            i++;

            archB.InitMap(seq,sortOutDeg);
            costB=archB.Alloc(seq);
            if(costB<mincost)
            {
                mincost=costB;
                minsort=sortOutDeg;
            }
        }

        temp=sortOutDeg[0];
        sortOutDeg[0]=sortOutDeg[1];
        sortOutDeg[1]=temp;
        i++;

        archB.InitMap(seq,sortOutDeg);
        costB=archB.Alloc(seq);
        if(costB<mincost)
        {
            mincost=costB;
            minsort=sortOutDeg;
        }

    }

    mincost=mincost+scount;
    cout << "Length of the sequence:" << seq.size()+scount << endl;
    cout << "Total Cost of HardwareA is: " << costA << endl;
    cout << "Total Cost of HardwareB is: " << mincost << endl;
    cout << "costB / costA = " << (double)mincost/costA << endl;

    for(i=0;i<minsort.size();i++)
        cout << minsort[i] << endl;

/*
    //RandSeqGen(seq,archA.GetQNum(),10000);
    GetSeq(seq,"seq/seq_inc_237.qasm");

    archA.InitMap(seq);
    costA=archA.Alloc(seq);

    archB.InitMap(seq);

    starttime=clock();

    costB=archB.Alloc(seq);

    endtime=clock();

    cout << "Length of the sequence:" << seq.size() << endl;
    cout << "Total Cost of HardwareA is: " << costA << endl;
    cout << "Total Cost of HardwareB is: " << costB << endl;
    cout << "Execution Time of B is:" << (double)(endtime-starttime)/CLOCKS_PER_SEC << endl;
    cout << "costB / costA = " << (double)costB/costA << endl;

/*
    string directory="/home/tilmto/CodeBlocks/QAX/seq";
    fcount=GetSeqList(fileList,directory);

    ofstream os("/home/tilmto/Ericpy/QuantumComputing/bridge/result",ios::out);

    for(int i=0; i<fcount; i++)
    {
        scount=GetSeq(seq,"seq/"+fileList[i]);

        archA.InitMap(seq);
        costA=archA.Alloc(seq)+scount;

        archB.InitMap(seq);

        starttime=clock();

        costB=archB.Alloc(seq)+scount;

        endtime=clock();

        os << fileList[i] << ":" << endl;
        os << "Length of the sequence:" << seq.size()+scount << endl;
        os << "Total Cost of HardwareA is: " << costA << endl;
        os << "Total Cost of HardwareB is: " << costB << endl;
        os << "Execution Time of B is: " << (double)(endtime-starttime)/CLOCKS_PER_SEC << endl;
        os << "costB / costA = " << (double)costB/costA << endl;
        os << endl;
    }

    os.close();
*/
    return 0;
}


void RandSeqGen(vector<vector<int>> &seq,int qubitNum,int seqLen)
{
    int cqubit,squbit;
    int i=0;
    srand((int)time(0));

    while(i<seqLen)
    {
        cqubit=rand()%qubitNum;
        squbit=rand()%qubitNum;
        if(cqubit!=squbit)
        {
            seq.push_back(vector<int>(2));
            seq[i][0]=cqubit;
            seq[i][1]=squbit;
            i++;
        }
    }
}


int GetSeq(vector<vector<int>> &seq,string fname)
{
    int i=0;
    int scount=0;
    int first,second;

    seq.clear();

    ifstream is(fname,ios::in);

    if(!is)
    {
        cout << "No such seq file." << endl;
        exit(1);
    }

    while(!is.eof())
    {
        is >> first;
        is >> second;
        if(first==-1)
            scount++;
        else
        {
            seq.push_back(vector<int>(2));
            seq[i][0]=first;
            seq[i][1]=second;
            i++;
        }
    }

    seq.pop_back();

    is.close();

    return scount;
}


void PrintSeq(vector<vector<int>> seq)
{
    cout << "Dependency Sequence:"<< endl;

    for(unsigned int i=0; i<seq.size(); i++)
        cout << "( " << seq[i][0] << " , " << seq[i][1] << " )" <<endl;
}


int GetSeqList(vector<string> &fileList, string directory)
{
    directory = directory.append("/");

    DIR *p_dir;
    const char* str = directory.c_str();

    p_dir = opendir(str);
    if( p_dir == NULL)
    {
        cout<< "can't open :" << directory << endl;
    }

    struct dirent *p_dirent;

    while ( p_dirent = readdir(p_dir))
    {
        string tmpFileName = p_dirent->d_name;
        if( tmpFileName == "." || tmpFileName == "..")
        {
            continue;
        }
        else
        {
            fileList.push_back(tmpFileName);
        }
    }
    closedir(p_dir);

    return fileList.size();
}


int frac(int n)
{
    if(n==0 || n==1)
        return 1;
    else
        return n*frac(n-1);
}




