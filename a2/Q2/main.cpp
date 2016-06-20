#include "systemc.h"
#include "producer.h"
#include "consumer.h"
#include "fifo.h"


int sc_main(int argc, char* argv[])
{
  sc_clock ClkFast("ClkFast", 50, SC_NS);
  sc_clock ClkSlow("ClkSlow", 100, SC_NS);

  fifo <float> Fifo1 ("F1", 16);
  consumer <float> C1("C1");
  C1.in(Fifo1);
  C1.Clock(ClkSlow);
  producer <float> P1("P1");
  P1.out(Fifo1);
  P1.Clock(ClkFast);
  

  sc_start(1001, SC_NS);

  return 0;
}

