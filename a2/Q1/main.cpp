//Cole McGinn V00780893

#include "systemc.h"

#define RDBLK 0
#define RDBYT 1
#define WTBYT 2
#define WTBLK 3

static sc_logic Z[8] = {SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z, SC_LOGIC_Z};

SC_MODULE(mem_ctrl) {

  sc_in < sc_uint<2> > comm;
  sc_in < sc_uint<8> > addr;
  sc_inout <sc_lv<8> > data;
  sc_inout <sc_lv<8> > dataBlk[4];

  sc_in_clk clk;
  sc_in <bool> reset;
  sc_in <bool> new_comm;
  sc_out <bool> complete;

  sc_lv <8> memory[256];	

  void mem_process() {	
  	sc_uint <2> comm_s;
  	sc_uint <8> addr_s;
  	sc_lv <8> data_s;
	while (true) {
	  if (reset.read() == true) complete.write(false);
	  else {
	    if (new_comm.read() == true) { 
		comm_s = comm.read();
		switch (comm_s) {
		  case RDBLK: 
		     	addr_s = addr.read();
			data_s = memory[addr_s];	  	
			cout << "@" << sc_time_stamp() 
	  		     << ": RDBLK-0, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			dataBlk[0].write(data_s);
			addr_s++;
		     	data_s = memory[addr_s];	  	
			cout << "@" << sc_time_stamp() 
	  		     << ": RDBLK-1, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			dataBlk[1].write(data_s); 
			addr_s++;
			data_s = memory[addr_s];  	
			cout << "@" << sc_time_stamp() 
	  		     << ": RDBLK-2, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			dataBlk[2].write(data_s); 
			addr_s++;
			data_s = memory[addr_s];	  	
			cout << "@" << sc_time_stamp() 
	  		     << ": RDBlk-3, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			dataBlk[3].write(data_s); 	
		  	break;
		  case RDBYT:
		     	addr_s = addr.read();
			data_s = memory[addr_s];	  	
			cout << "@" << sc_time_stamp() 
	  		     << ": RDBYT, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			data.write(data_s);
		  	break;
		  case WTBYT:
		  	addr_s = addr.read();
			data_s = data.read();
			cout << "@" << sc_time_stamp() 
	  		     << ": WTBYT, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait(); 
		  	memory[addr_s] = data_s;
			break;
		  case WTBLK:
		  	addr_s = addr.read();
			data_s = data.read();
			cout << "@" << sc_time_stamp() 
	  		     << ": WTBLK-0, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			memory[addr_s] = data_s;
			addr_s++;		
			data_s = data.read();
			cout << "@" << sc_time_stamp() 
	  		     << ": WTBLK-1, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			memory[addr_s] = data_s;
			addr_s++;
			data_s = data.read();
			cout << "@" << sc_time_stamp() 
	  		     << ": WTBLK-2, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			memory[addr_s] = data_s;
			addr_s++;
			data_s = data.read();
			cout << "@" << sc_time_stamp() 
	  		     << ": WTBLK-3, address = " << addr_s 
			     << ", data = " << data_s << endl;
		  	wait();
			memory[addr_s] = data_s;	     
		  	break;
		  default: 
		  	cout << "Illegal command : " << comm_s << endl;
		  	break;
		}
		complete.write(true);
		while (new_comm.read() == true) {
			if (reset.read() == true) break;
			wait();
		}
		// stop driving
		if (comm_s == RDBYT) data.write(Z);
		else if (comm_s == RDBLK){
			int x=0;
			for(x=0;x<4;x++){
				dataBlk[x].write(Z);
			}
		}
		complete.write(false);
	    }
	  }
	  wait();
	}
  }

  SC_CTOR(mem_ctrl) { 
	SC_CTHREAD(mem_process, clk.pos()); 
  }

};


SC_MODULE(mem_testbench) {

	sc_out < sc_uint<2> > comm;
	sc_out < sc_uint<8> > addr;
	sc_inout < sc_lv<8> > data;
	sc_inout <sc_lv<8> > dataBlk[4];

	sc_in_clk clk;
	sc_out <bool> reset;
	sc_out <bool> new_comm;
	sc_in <bool> complete;

void handshake(){
	new_comm.write(true);
	while(complete.read() == false) wait();
	if (comm.read() == RDBYT)
		cout << "byte read = " << data.read() << endl;
	else if (comm.read() == RDBLK){
		int x =0;
		for(x=0;x<4;x++){
			cout << "block read "<< x <<" = "<< dataBlk[x].read() << endl;
		}
	} else data.write(Z);
	new_comm.write(false);
	while(complete.read() == true) wait();
}

void testbench() {

	reset.write(true);
	wait();
	
	reset.write(false);
	
	addr.write(128);
	comm.write(RDBYT);
	handshake();

	addr.write(128);
	data.write(255);
	comm.write(WTBYT);	
	handshake();

	addr.write(128);
	comm.write(RDBYT);
	handshake();

	addr.write(128);
	data.write(1);
	comm.write(WTBLK);	
	handshake();
	
	addr.write(128);
	data.write(51);
	comm.write(WTBYT);	
	handshake();
	addr.write(129);
	data.write(123);
	comm.write(WTBYT);	
	handshake();
	addr.write(130);
	data.write(5);
	comm.write(WTBYT);	
	handshake();
	
	addr.write(128);
	comm.write(RDBLK);	
	handshake();

	addr.write(43);
	comm.write(RDBYT);	
	handshake();

	addr.write(43);
	data.write(0);
	comm.write(WTBYT);	
	handshake();	

	addr.write(43);
	comm.write(RDBYT);	
	handshake();

	addr.write(22);
	comm.write(RDBLK);	
	handshake();	

	addr.write(23);
	data.write(255);
	comm.write(WTBYT);	
	handshake();	

	addr.write(25);
	data.write(101);
	comm.write(WTBYT);	
	handshake();
	
	addr.write(22);
	comm.write(RDBLK);	
	handshake();		
	
	addr.write(0);
	data.write(129);
	comm.write(WTBLK);
	handshake();	

	addr.write(0);
	comm.write(RDBYT);	
	handshake();

	addr.write(3);
	comm.write(RDBYT);	
	handshake();

				
  }

  SC_CTOR(mem_testbench) { 
	SC_CTHREAD(testbench, clk.pos()); 
  }

};


int sc_main(int argc, char* argv[])
{

mem_ctrl MC("MC");
mem_testbench TB("TB");

    sc_signal <bool> reset, new_comm, complete;
    sc_signal < sc_uint<2> > comm;
    sc_signal < sc_uint<8> > addr;
    sc_signal_rv <8> data;
    sc_signal_rv <8> dataBlk[4];
    sc_clock TestClk("TestClock",10,SC_NS,0.5,1,SC_NS);
    // _rv needed because of multiple drivers
  

    // Interconnects:
    MC.clk(TestClk);
    MC.reset(reset);
    MC.data(data);
    int x =0;
	for(x=0;x<4;x++){
    MC.dataBlk[x](dataBlk[x]);
    TB.dataBlk[x](dataBlk[x]);
	}
    MC.addr(addr);
    MC.comm(comm);
    MC.new_comm(new_comm);
    MC.complete(complete);

    TB.clk(TestClk);
    TB.reset(reset);
    TB.data(data);
    
    TB.addr(addr);
    TB.comm(comm);
    TB.new_comm(new_comm);
    TB.complete(complete);

    sc_start(1100, SC_NS);

    return 0;
}
