#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor() : total_prev_non_idle_(0), total_prev_idle_(0){};
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long total_prev_non_idle_;
  long total_prev_idle_;
};

#endif