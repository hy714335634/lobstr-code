/*
 Copyright (C) 2011 Melissa Gymrek <mgymrek@mit.edu>
*/

#ifndef SRC_TEXTFILEREADER_H__
#define SRC_TEXTFILEREADER_H__

#include <istream>
#include <fstream>
#include <string>

#include "src/IFileReader.h"

class TextFileReader : public IFileReader {
 public:
  explicit TextFileReader(const std::string& _filename="");
  virtual ~TextFileReader();
  virtual bool GetNextRead(MSReadRecord* read);
  virtual bool GetNextRecord(ReadPair* read_pair);
  bool GetNextLine(std::string* line);

 protected:
  size_t current_line;
  std::string filename;
  std::ifstream *input_file_stream;
  std::istream &input_stream;
  static std::ifstream* create_file_stream(const std::string& filename);
};

#endif  // SRC_TEXTFILEREADER_H__
