/*
 Copyright (C) 2011 Melissa Gymrek <mgymrek@mit.edu>
*/

#include <err.h>
#include "common.h"
#include "FastaPairedFileReader.h"
#include "runtime_parameters.h"
#include "ZippedFastaFileReader.h"

using namespace std;

FastaPairedFileReader::FastaPairedFileReader(const string& _filename1,
					     const string& _filename2) {
  if (gzip) {
    _reader1 = new ZippedFastaFileReader(_filename1);
    _reader2 = new ZippedFastaFileReader(_filename2);
  } else {
    _reader1 = new FastaFileReader(_filename1);
    _reader2 = new FastaFileReader(_filename2);
  }
}

bool FastaPairedFileReader::GetNextRecord(ReadPair* read_pair) {
  read_pair->reads.clear();
  MSReadRecord read1;
  MSReadRecord read2;
  if (_reader1->GetNextRead(&read1)) {
    read_pair->reads.push_back(read1);
  } else {
    return false;
  }
  if (_reader2->GetNextRead(&read2)) {
    read_pair->reads.push_back(read2);
  } else {
    return false;
  }
  return true;
}

// Does not apply for paired reads reader
bool FastaPairedFileReader::GetNextRead(MSReadRecord* read) {
  return false;
}
