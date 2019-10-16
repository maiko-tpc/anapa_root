#include "myana.hpp"
#include "myfunc.h"
#include <TROOT.h>
#include <TF1.h>
#include <TGraph.h>
#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "Dict.h"

MyAna::MyAna()
{
}

MyAna::~MyAna()
{
}

void MyAna::Analysis(struct evtdata *evt)
{
  InitLocal();
// Find edges of tracks
  std::vector<std::vector<double> > m_edge;
  std::vector<double> x;
  int track_state = 0;
  for(int i=0;i!=N_AC;++i){
    std::vector<std::vector<int> > l_edge_ac;
    std::vector<std::vector<int> > t_edge_ac;
    std::vector<double> m_edge_ac;
    for(int j=0;j!=N_STRP;++j){
      std::vector<int> l_edge_strp;
      std::vector<int> t_edge_strp;
      if(evt->imtpc[i][0][j]==1){
	l_edge_strp.push_back(0);
      }
      for(int k=1;k!=N_TCLK;++k){
	if(evt->imtpc[i][k][j]!=0){
	  Map[i][j][k/64] += (((ULong64_t)1)<<(k%64));
	  switch(i){
	  case 0:
	    map_a->Fill(j, k, 1.);
	    break;
	  case 1:
	    map_c->Fill(j, k, 1.);
	    break;
	  }
	}
	if(evt->imtpc[i][k-1][j]!=evt->imtpc[i][k][j]){
	  switch(evt->imtpc[i][k][j]){
	  case 0:
	    if(track_state==0){
	      l_edge_strp.push_back(k);
	    }
	    track_state = 1;
	    break;
	  case 1:
	    if(track_state==1){
	      t_edge_strp.push_back(k);
	    }
	    track_state = 0;
	    break;
	  }
	}
      }
      if(evt->imtpc[i][1023][j]==1){
	t_edge_strp.push_back(1023);
      }
      l_edge_ac.push_back(l_edge_strp);
      t_edge_ac.push_back(t_edge_strp);

//      l_edge[i][j] = l_edge_strp;
//      t_edge[i][j] = t_edge_strp;

      if((l_edge_ac.end()-1)->size() && (t_edge_ac.end()-1)->size()){
	m_edge_ac.push_back(((t_edge_ac.end()-1)->at(0)+(l_edge_ac.end()-1)->at(0))*0.5);
	x.push_back(j);
      }
    }
    l_edge.push_back(l_edge_ac);
    t_edge.push_back(t_edge_ac);
    m_edge.push_back(m_edge_ac);
//    edge_pos[0][ac] = l_edge_ac.data();
//    edge_pos[1][ac] = t_edge_ac.data();
//    edge_num
  }

// Calc drift speed
  TGraph gr(x.size(), x.data(), m_edge[0].data());
  TF1 f = TF1("f", "[0]*x+[1]", 0, 256);
  f.SetParameter(0, -1);  // Set initial values of fitting function
  f.SetParameter(1, 300);
  gr.Fit("f", "RNQ");
  p[0] = f.GetParameter(0); // Get fitting values
  p[1] = f.GetParameter(1);
  p[2] = f.GetChisquare()/f.GetNDF();
  if(p[0]>-0.1 && p[0]<0.1){
    drift_v = -1;
  }else{
    drift_v = TMath::Tan(30.*TMath::DegToRad())*0.4/(10*TMath::Abs(p[0]));
  }
  
// Integrate FADC
  int th = 0;
  for(int ac=0;ac!=N_AC;++ac){
    integ_fadc_full[ac] = 0;
    for(int str=0;str!=N_FADC;++str){;
      integ_fadc[ac][str] = 0;
      for(int clk=0;clk!=5;++clk){
	base_line[ac][str] += evt->icadc[ac][str][clk];
      }
      base_line[ac][str] /= 5.;
      for(int clk=5;clk!=N_ACLK;++clk){
	if((evt->icadc[ac][str][clk]-base_line[ac][str])*TMath::Power(-1, ac)>th){
	  integ_fadc[ac][str] += (evt->icadc[ac][str][clk]-base_line[ac][str])*TMath::Power(-1, ac);
	}
      }
      integ_fadc_full[ac] += integ_fadc[ac][str];
    }
  }

// Find peak
  for(int ac=0;ac!=N_AC;++ac){
    for(int str=0;str!=N_FADC;++str){
      peak[ac][str] = 0;
      for(int clk=0;clk!=N_ACLK;++clk){
	if((evt->icadc[ac][str][clk]-base_line[ac][str])*TMath::Power(-1,ac)>peak[ac][str]){
	  peak[ac][str] = (evt->icadc[ac][str][clk]-base_line[ac][str])*TMath::Power(-1, ac);
	}
      }
    }
  }

  
  for(int ch=0;ch!=N_SCA;++ch){
    sca[ch] = evt->isca[ch];
  }

  // tpc clk
  if(tpc_clk==0){
    tpc_clk = (double)evt->start_time;
  }

  if(evt->tpcclk[0][1]>evt->tpcclk[0][0]){
    tpc_clk += (evt->tpcclk[0][1]-evt->tpcclk[0][0])/1.e8; // clk is 100 MHz?
  }else{
    tpc_clk += ((0xfffffff+evt->tpcclk[0][1])-evt->tpcclk[0][0])/1.e8;
  }

  ++evt_num;
  return;
}

void MyAna::TreeDef()
{
  /* TTree definition */
  char tname[10] = "tree";
  tree = new TTree(tname, tname);  
  char branch[256];

  tree->Branch("evt_num", &evt_num, "evt_num/I");
//  tree->Branch("tpc_fit_para", tpc_fit_para, "tpc_fit_para[2][2]/D");
  tree->Branch("tpc_clk", &tpc_clk, "tpc_clk/D");
  tree->Branch("p", p, "p[3]/D");
  tree->Branch("drift_v", &drift_v, "drift_v/D");
  tree->Branch("fit_para", p, "fit_para[3]/D");
  sprintf(branch, "integ_fadc[%d][%d]/D", N_AC, N_FADC);
  tree->Branch("integ_fadc", integ_fadc, branch);
  sprintf(branch, "integ_fadc_full[%d]/D", N_AC);
  tree->Branch("integ_fadc_full", integ_fadc_full, branch);
  sprintf(branch, "peak[%d][%d]/D", N_AC, N_FADC);
  tree->Branch("peak", peak, branch);
  sprintf(branch, "sca[%d]/I", N_SCA);
  tree->Branch("sca", sca, branch);
  tree->Branch("Map", Map, "Map[2][256][16]/l");
//  tree->Branch("l_edge", &l_edge);
//  tree->Branch("t_edge", &t_edge);
//  tree->Branch("l_edge", "std::vector<int>[2][256]", l_edge);
//  tree->Branch("t_edge", "std::vector<int>[2][256]", t_edge);

/* TH1 definition */

/* TH2 definition */
  map_a = new TH2D("map_a", "map_a", 256, 0, 256, 1024, 0, 1024);
  map_c = new TH2D("map_c", "map_c", 256, 0, 256, 1024, 0, 1024);
  hist2d.push_back(map_a);
  hist2d.push_back(map_c);
}

void MyAna::TreeFill()
{
  tree->Fill();
  save_flag = 0;
  return;
}

void MyAna::AutoSave()
{
  if(!save_flag && !(evt_num%100)){
    file->cd();
    tree->AutoSave("SaveSelf");
    save_flag = 1;
  }
  return;
}

void MyAna::MakeFile(char *fname)
{
  std::string filename = fname;
  filename = filename.substr(0, filename.find_last_of("."))+".root";
  file = new TFile(filename.c_str(), "RECREATE");
}

void MyAna::CloseFile()
{
  if(!save_flag){
    file->cd();
    tree->AutoSave();
  }
  for(std::vector<TH1D*>::iterator it=hist1d.begin();it!=hist1d.end();++it){
    (*it)->Write();
  }
  for(std::vector<TH2D*>::iterator it=hist2d.begin();it!=hist2d.end();++it){
    (*it)->Write();
  }
  file->Close();
}

void MyAna::Init()
{
  tpc_clk = 0;
  save_flag = 0;
  evt_num = 0;
}

void MyAna::InitLocal()
{
  for(int ac=0;ac!=2;++ac){
    for(int strp=0;strp!=256;++strp){
      for(int buf=0;buf!=16;++buf){
	Map[ac][strp][buf] = 0;
      }
    }
  }
  l_edge.clear();
  t_edge.clear();
  for(int ii=0;ii!=3;++ii){
    p[ii] = 0;
  }
  drift_v = 0;
  for(int ii=0;ii!=N_AC;++ii){
    for(int jj=0;jj!=N_FADC;++jj){
      integ_fadc[ii][jj] = 0;
      base_line[ii][jj] = 0;
    }
    integ_fadc_full[ii] = 0;
  }
}
