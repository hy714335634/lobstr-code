/*
Copyright (C) 2011 Melissa Gymrek <mgymrek@mit.edu>

This file is part of lobSTR.

lobSTR is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

lobSTR is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with lobSTR.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef SRC_BWAREADALIGNER_H_
#define SRC_BWAREADALIGNER_H_

#include <map>
#include <string>
#include <vector>

#include "src/cigar.h"
#include "src/common.h"
#include "src/ReadPair.h"

// Store a single alignment found by BWA
struct ALIGNMENT {
  // Identifier of the STR aligned to
  int id;
  // Chrom of the STR aligned to
  std::string chrom;
  // Start of the STR
  int start;
  // End of the STR
  int end;
  // This alignment is on the left side
  bool left;
  // Repeat motif
  std::string repeat;
  // true = positive, false = minus
  bool strand;
  // Position of the start of the alignment
  int pos;
  // Position of the end of the alignment
  int endpos;
  // Copy number
  float copynum;
  // provide overloaded operators to compare
  // when used as a map key
  bool operator<(const ALIGNMENT& ref_pos1) const {
    if (chrom != ref_pos1.chrom) return true;
    if (start == ref_pos1.start) {
      return end < ref_pos1.end;
    } else {
      return start < ref_pos1.start;
    }
  }
};

class BWAReadAligner {
  friend class BWAReadAlignerTest;
 public:
  BWAReadAligner(BWT* bwt_reference,
                 BNT* bnt_annotation,
                 std::map<int, REFSEQ>* ref_sequences,
                 gap_opt_t *opts);
  virtual ~BWAReadAligner();

  // main functions - align read pair
  bool ProcessReadPair(ReadPair* read_pair, std::string* err, std::string* messages);
  bool ProcessPairedEndRead(ReadPair* read_pair, std::string* err, std::string* messages);
  bool ProcessSingleEndRead(ReadPair* read_pair, std::string* err, std::string* messages);

 protected:
  // Check if flanking regions fully repetitive
  bool CheckFlanksForRepeats(MSReadRecord* read, const std::string& repseq);

  // Process a single read of a pair
  // Return possible alignments of flanking regions
  // in "good_*_alignments"
  bool ProcessRead(MSReadRecord* read,
		   bool passed_detection,
                   std::vector<ALIGNMENT>* good_left_alignments,
                   std::vector<ALIGNMENT>* good_right_alignments,
                   std::string* err,
                   std::string* messages);

  // Set up sequence info for BWA
  void SetSeq(bwa_seq_t* seq, const std::string& flank_nuc,
	      const std::string& flank_qual, const std::string& readid);

  // Call BWA to align flanking regions
  bwa_seq_t* BWAAlignFlanks(const MSReadRecord& read);

  // Get info from ref fields of index
  void ParseRefid(const std::string& refstring, ALIGNMENT* refid);

  // Get the coordinates of each alignment
  bool GetAlignmentCoordinates(bwa_seq_t* aligned_seqs,
                               const std::string& repseq,
                               std::vector<ALIGNMENT>* alignments);

  // Get a unique shared alignment between left and right flanks
  // Output unique left and right alignments in *_refids
  bool GetSharedAlns(const std::vector<ALIGNMENT>& map1,
                     const std::vector<ALIGNMENT>& map2,
                     std::vector<ALIGNMENT>* left_refids,
                     std::vector<ALIGNMENT>* right_refids);

  // Set STR coordinates of shared alignments
  bool SetSTRCoordinates(std::vector<ALIGNMENT>* good_left_alignments,
			 std::vector<ALIGNMENT>* good_right_alignments);

  // Get list of reference STRs spanned by a set of alignments
  void GetSpannedSTRs(const ALIGNMENT& lalign, const ALIGNMENT& ralign, const int& refid,
		      std::vector<ReferenceSTR>* spanned_ref_strs, std::vector<string>* repseq);

  // Trim mate sequence
  void TrimMate(ReadPair* read_pair);

  // Align mate
  bool AlignMate(const ReadPair& read_pair,
                 std::vector<ALIGNMENT>* mate_alignments,
                 const std::string& repseq);

  // Check that the mate pair maps to the same region
  // If yes, update mate_alignment info
  bool CheckMateAlignment(const std::vector<ALIGNMENT>& mate_alignments,
                          const ALIGNMENT& left_alignment,
                          const ALIGNMENT& right_alignment,
                          ALIGNMENT* mate_alignment);

  // Try stitching a pair of reads.
  // Update info in num_aligned_read and
  // treat as single alignment
  bool StitchReads(ReadPair* read_pair,
                   ALIGNMENT* left_alignment,
                   ALIGNMENT* right_alignment);

  // Output fields for successful alignment
  bool OutputAlignment(ReadPair* read_pair,
                       const ALIGNMENT& left_alignment,
                       const ALIGNMENT& right_alignment,
                       const ALIGNMENT& mate_alignment,
		       const std::string& alternate_mappings,
                       bool treat_as_paired);

  // Perform local realignment, adjust exact STR boundaries
  // update cigar score.
  bool AdjustAlignment(MSReadRecord* aligned_read);

  // Calculate map quality score
  int GetMapq(const std::string& aligned_sw_string,
              const std::string& ref_sw_string,
              const std::string& aligned_quals,
              int* edit_dist);

  // Refine the cigar score and recalculate number of repeats
  bool GetSTRAllele(MSReadRecord* aligned_read,
                    const CIGAR_LIST& cigar_list);
  // store all BWT references
  BWT* _bwt_reference;
  // store all BWT annotations
  BNT* _bnt_annotation;
  // store all STR reference sequences
  std::map<int, REFSEQ>* _ref_sequences;
  // all bwa alignment options
  gap_opt_t *_opts;
  // default options
  gap_opt_t *_default_opts;

  // Debug params
  bool cigar_debug;
  bool stitch_debug;
};

#endif  // SRC_BWAREADALIGNER_H_
