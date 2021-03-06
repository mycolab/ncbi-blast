/*  $Id: blast_aux_priv.hpp 617919 2020-10-08 11:57:15Z gouriano $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *               National Center for Biotechnology Information
 *
 *  This software/database is a "United States Government Work" under the
 *  terms of the United States Copyright Act.  It was written as part of
 *  the author's official duties as a United States Government employee and
 *  thus cannot be copyrighted.  This software/database is freely available
 *  to the public for use. The National Library of Medicine and the U.S.
 *  Government have not placed any restriction on its use or reproduction.
 *
 *  Although all reasonable efforts have been taken to ensure the accuracy
 *  and reliability of the software and data, the NLM and the U.S.
 *  Government do not and cannot warrant the performance or results that
 *  may be obtained by using this software or data. The NLM and the U.S.
 *  Government disclaim all warranties, express or implied, including
 *  warranties of performance, merchantability or fitness for any particular
 *  purpose.
 *
 *  Please cite the author in any work or product based on this material.
 *
 * ===========================================================================
 *
 * Author:  Christiam Camacho
 *
 */

/** @file blast_aux_priv.hpp
 * Auxiliary functions for BLAST
 */

#ifndef ALGO_BLAST_API___BLAST_AUX_PRIV__HPP
#define ALGO_BLAST_API___BLAST_AUX_PRIV__HPP

#include <corelib/ncbiobj.hpp>
#include <algo/blast/api/blast_types.hpp>
#include <algo/blast/api/blast_results.hpp>
#include <objtools/blast/seqdb_reader/seqdb.hpp>
#include <algo/blast/api/query_data.hpp>        // for IQueryFactory
#include <algo/blast/api/blast_options.hpp>     // for CBlastOptions
#include <algo/blast/api/setup_factory.hpp>     // for SInternalData
#include "split_query.hpp"                      // for CQuerySplitter

/** @addtogroup AlgoBlast
 *
 * @{
 */

struct BlastSeqSrc;
struct Blast_Message;
struct BlastQueryInfo;

BEGIN_NCBI_SCOPE

BEGIN_SCOPE(objects)
    class CSeq_id;
    class CSeq_loc;
END_SCOPE(objects)

BEGIN_SCOPE(blast)

/** Create a single CSeq_loc of type whole from the first id in the list.
 * @param seqids identifiers for the Seq-loc [in]
 */
CConstRef<objects::CSeq_loc> 
CreateWholeSeqLocFromIds(const list< CRef<objects::CSeq_id> > seqids);

// Auxiliary comparison functors for TQueryMessages (need to dereference the
// CRef<>'s contained)
struct TQueryMessagesLessComparator
{ 
    /// Operator to determine if *a is less than *b
    bool operator() (const CRef<CSearchMessage>& a,
                     const CRef<CSearchMessage>& b) const {
        return *a < *b;
    }
};

struct TQueryMessagesEqualComparator
{ 
    /// Operator to determine if *a is equal to *b
    bool operator() (const CRef<CSearchMessage>& a,
                     const CRef<CSearchMessage>& b) const {
        return *a == *b;
    }
};

/// Converts the Blast_Message structure into a TSearchMessages object.
void
Blast_Message2TSearchMessages(const Blast_Message* blmsg,
                              const BlastQueryInfo* query_info,
                              TSearchMessages& messages);

/// Returns a string containing a human-readable interpretation of the
/// error_code passed as this function's argument
string
BlastErrorCode2String(Int2 error_code);

/// Return type of BlastSetupPreliminarySearch
struct SBlastSetupData : public CObject {
    SBlastSetupData(CRef<IQueryFactory> qf,
                    CRef<CBlastOptions> opts)
    : m_InternalData(new SInternalData), 
      m_QuerySplitter(new CQuerySplitter(qf, opts))
    {}

    CRef<SInternalData> m_InternalData;
    CRef<CQuerySplitter> m_QuerySplitter;
    TSeqLocInfoVector m_Masks;
    TSearchMessages m_Messages;
};


/// Set up internal data structures used by the BLAST CORE engine
/// @param query_factory source of query sequence data structures [in]
/// @param options BLAST options [in]
/// @param num_threads number of threads [in]
CRef<SBlastSetupData>
BlastSetupPreliminarySearch(CRef<IQueryFactory> query_factory,
                            CRef<CBlastOptions> options,
                            size_t num_threads = 1);


/// Extended interface to set up internal data structures used by the BLAST
/// CORE engine
/// @param query_factory source of query sequence data structures [in]
/// @param options BLAST options [in]
/// @param pssm PSSM [in]
/// @param seqsrc source of database/subject sequence data [in]
/// @param num_threads number of threads to use [in]
CRef<SBlastSetupData>
BlastSetupPreliminarySearchEx(CRef<IQueryFactory> qf,
                              CRef<CBlastOptions> options,
                              CConstRef<CPssmWithParameters> pssm,
                              BlastSeqSrc* seqsrc,
                              size_t num_threads);

/// Builds an CSearchResultSet::TAncillaryVector
/// @param program BLAST program [in]
/// @param query_ids the sequence identifiers for the queries [in]
/// @param sbp BlastScoreBlk structure used during the search [in]
/// @param qinfo BlastQueryInfo structure used during the search [in]
/// @param alignments alignments that were the result of the search [in]
/// @param result_type are these results for bl2seq or a database search? [in]
/// @param retval the output of this function is returned through this
/// parameter [in|out]
void
BuildBlastAncillaryData(EBlastProgramType program,
                        const vector< CConstRef<CSeq_id> >& query_ids,
                        const BlastScoreBlk* sbp,
                        const BlastQueryInfo* qinfo,
                        const TSeqAlignVector& alignments,
                        const EResultType result_type,
                        CSearchResultSet::TAncillaryVector& retval);

/// Build a CSearchResultSet from internal BLAST data structures
/// @param query_masks Masks for query sequences [in]
/// @param subj_masks Masks for subject sequences [in]
CRef<CSearchResultSet>
BlastBuildSearchResultSet(const vector< CConstRef<CSeq_id> >& query_ids,
                          const BlastScoreBlk* sbp,
                          const BlastQueryInfo* qinfo,
                          EBlastProgramType program,
                          const TSeqAlignVector& alignments,
                          TSearchMessages& messages,
                          const vector<TSeqLocInfoVector>& subj_masks,
                          const TSeqLocInfoVector* query_masks = NULL,
          const EResultType result_type = ncbi::blast::eDatabaseSearch);

END_SCOPE(blast)
END_NCBI_SCOPE

/* @} */

#endif  /* ALGO_BLAST_API___BLAST_AUX_PRIV__HPP */
