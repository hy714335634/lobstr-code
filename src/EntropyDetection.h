/*
 Copyright (C) 2011 Melissa Gymrek <mgymrek@mit.edu>
*/

#ifndef SRC_ENTROPYDETECTION_H__
#define SRC_ENTROPYDETECTION_H__

#include <string>
#include <vector>

class EntropyDetection {
 private:
  std::string _nucs;
  std::vector<double> _entropy_window;
  int _window_size;
  int _window_step;

 public:
  EntropyDetection(const std::string& nucleotides, int size, int step);
  ~EntropyDetection();
  // dinuc entropy, not generalized but optimized
  double EntropyOneWindowDinuc(const std::string& window_nucs);

  // vector of entropy values for each window
  void CalculateEntropyWindow();

  // determine if there is a window above the entropy threshold
  bool EntropyIsAboveThreshold();

  // determine start and end region
  void FindStartEnd(size_t* start, size_t* end, bool* repetitive_end);
};

#endif  // SRC_ENTROPYDETECTION_H__
