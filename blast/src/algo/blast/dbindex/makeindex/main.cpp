/*  $Id: main.cpp 492284 2016-02-16 16:55:37Z camacho $
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
 * Author:  Aleksandr Morgulis
 *
 * File Description:
 *   The main() routine of mkindex application.
 *
 */

#include <ncbi_pch.hpp>

#ifdef LOCAL_SVN

#include "../libindexdb/sequence_istream_fasta.hpp"
#include "../libindexdb/dbindex.hpp"

#else

#include <algo/blast/dbindex/sequence_istream_fasta.hpp>
#include <algo/blast/dbindex/dbindex.hpp>

#endif

#include "mkindex_app.hpp"

USING_NCBI_SCOPE;

int main( int argc, char * argv[] )
{ 
    return CMkIndexApplication().AppMain(argc, argv);
}
