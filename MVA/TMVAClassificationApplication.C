/**********************************************************************************
 * Project   : TMVA - a Root-integrated toolkit for multivariate data analysis    *
 * Package   : TMVA                                                               *
 * Exectuable: TMVAClassificationApplication                                      *
 *                                                                                *
 * This macro provides a simple example on how to use the trained classifiers     *
 * within an analysis module                                                      *
 **********************************************************************************/

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TChain.h"
#include "TStopwatch.h"

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#endif

using namespace TMVA;

void TMVAClassificationApplication( int index=0) 
{   
    TString af[2]={
        "../DataRunIUpdate.root",
        "../MCRunIUpdate.root"
    };
    TString input_file_name=af[index];
    TString st;
    TMVA::Tools::Instance();

    ifstream fconf("MethodConf.txt",ios::in);
    string method;
    int use;
    std::map<std::string,int> Use;
    while(true){
        fconf>>method>>use;
        if(fconf.eof()) break;
        Use[method] = use;
    }


    TMVA::Reader *reader1 = new TMVA::Reader( "!Color:!Silent" );    
    TMVA::Reader *reader2 = new TMVA::Reader( "!Color:!Silent" );    
    TMVA::Reader *reader3 = new TMVA::Reader( "!Color:!Silent" );    

    // Create and declare to reader variables, MUST same orders names and types as weights
    Float_t    minPT, LogminIPCHI2, minPID, gamma_CL, gamma_PT, Bp_IPCHI2_OWNPV, Bp_PT, LogBp_FDCHI2_OWNPV, Bp_ConstChic1Fit_chi2;


    reader1->AddVariable( "minPT", &minPT);
    reader1->AddVariable( "log(minIPCHI2)", &LogminIPCHI2);
    reader1->AddVariable( "minPID", &minPID);
    reader1->AddVariable( "gamma_CL", &gamma_CL);
    reader1->AddVariable( "gamma_PT", &gamma_PT);
    reader1->AddVariable( "Bp_IPCHI2_OWNPV", &Bp_IPCHI2_OWNPV);
    reader1->AddVariable( "Bp_PT", &Bp_PT);
    reader1->AddVariable( "log(Bp_FDCHI2_OWNPV)", &LogBp_FDCHI2_OWNPV);
    reader1->AddVariable( "Bp_ConstChic1Fit_chi2[0]", &Bp_ConstChic1Fit_chi2);


    // --- Book the MVA methods

    TString dir    = "dataset/weights/";
    TString prefix1 = st.Format("TMVAClassification");

    // Book method(s)
    for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
        if (it->second) {
            TString methodName = TString(it->first) + TString(" method");
            TString weightfile1 = dir + prefix1 + TString("_") + TString(it->first) + TString(".weights.xml");
            reader1->BookMVA( methodName, weightfile1 ); 
        }
    }



    TFile *input = new TFile(input_file_name);
    TTree* theTree = (TTree*)input->Get("DecayTree");
    std::cout << "MVA only supports float variables??, make sure you convert::::::::" << std::endl;

    Double_t minPT_t, LogminIPCHI2_t, minPID_t, gamma_CL_t, gamma_PT_t, Bp_IPCHI2_OWNPV_t, Bp_PT_t, LogBp_FDCHI2_OWNPV_t; 
    Float_t *Bp_ConstChic1Fit_chi2_t;
    theTree->SetBranchAddress( "minPT", &minPT_t);
    theTree->SetBranchAddress( "minIPCHI2", &LogminIPCHI2_t);
    theTree->SetBranchAddress( "minPID", &minPID_t);
    theTree->SetBranchAddress( "gamma_CL", &gamma_CL_t);
    theTree->SetBranchAddress( "gamma_PT", &gamma_PT_t);
    theTree->SetBranchAddress( "Bp_IPCHI2_OWNPV", &Bp_IPCHI2_OWNPV_t);
    theTree->SetBranchAddress( "Bp_PT", &Bp_PT_t);
    theTree->SetBranchAddress( "Bp_FDCHI2_OWNPV", &LogBp_FDCHI2_OWNPV_t);
    theTree->SetBranchAddress( "Bp_ConstChic1Fit_chi2", Bp_ConstChic1Fit_chi2_t);

    TString __dir = TString(input_file_name(0,input_file_name.Last('/')+1));
    TString __file = TString(input_file_name(input_file_name.Last('/')+1,input_file_name.Last('.')-input_file_name.Last('/')-1));
    cout<<"__file: "<<__file<<endl;
    __file = __file.ReplaceAll(TString(".root"),TString(""));
    TString output_file_name = __dir+__file+"MVA.root";
    TFile *newfile = new TFile(output_file_name,"recreate");
    TTree *newtree = theTree->CloneTree(0);

    Double_t responses1[100]={0.};
    for(int i=0;i<100;i++) responses1[i]=-1000.;
    int Nused=0;
    for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
        if (it->second) {
            TString tmp = it->first;
            //TString tmp = "MVA_"+it->first+"_response";
            newtree->Branch(tmp,responses1+Nused,tmp+"/D");
            Nused++;
        }
    }

    std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
    for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {
        if (ievt%1000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;
        theTree->GetEntry(ievt);
         minPT                  =   minPT_t;
         LogminIPCHI2           =   log(LogminIPCHI2_t);
         minPID                 =   minPID_t;
         gamma_CL               =   gamma_CL_t;
         gamma_PT               =   gamma_PT_t;
         Bp_IPCHI2_OWNPV        =   Bp_IPCHI2_OWNPV_t;
         Bp_PT                  =   Bp_PT_t;
         LogBp_FDCHI2_OWNPV     =   log(LogBp_FDCHI2_OWNPV_t);
         Bp_ConstChic1Fit_chi2  =   Bp_ConstChic1Fit_chi2_t[0];
        //For the same group of events in the training, so apply the same cuts
        Nused=0;
        for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
            if (it->second) {
                responses1[Nused] = double(reader1->EvaluateMVA(it->first+" method"));
                Nused++;
            }
        }

        newtree->Fill();
    }


    newtree->AutoSave();
    newfile->Close();
    input->Close();

    std::cout << "--- Created root file: "<<newfile->GetName()<<" containing the MVA output and a copy of input TTree" << std::endl;

    std::cout << "==> TMVAClassificationApplication is done!" << endl << std::endl;
    delete reader1;
    delete newfile;
    delete input;
} 
