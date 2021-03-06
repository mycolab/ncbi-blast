/*
 * $Id: reader_message_handler.cpp 632526 2021-06-02 17:25:01Z ivanov $
 *
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
 * Authors: Frank Ludwig
 *
 */
#include <ncbi_pch.hpp>
#include <objtools/readers/line_error.hpp>
#include <objtools/readers/reader_listener.hpp>
#include "reader_message_handler.hpp"

BEGIN_NCBI_SCOPE
BEGIN_SCOPE(objects);

class ILineErrorListener;

//  ============================================================================
void
CReaderMessageHandler::Report(
    const CReaderMessage& readerMessage)
//  ============================================================================
{
    if (mpRL) {
        mpRL->PutMessage(readerMessage);
        return;
    }
    AutoPtr<CObjReaderLineException> pErr(
        CObjReaderLineException::Create(
        readerMessage.Severity(),
        readerMessage.LineNumber(),
        readerMessage.Message(),
        ILineError::eProblem_GeneralParsingError));
    pErr->Throw();
}


//  ============================================================================
void
CReaderMessageHandler::Progress(
    const CReaderProgress& readerProgress)
//  ============================================================================
{
    if (mpRL) {
        mpRL->Progress(readerProgress);
        return;
    }
}


//  =============================================================================
size_t
CReaderMessageHandler::LevelCount(
    EDiagSev severity) const
//  =============================================================================
{
    if (!mpRL) {
        return 0;
    }
    return mpRL->LevelCount(severity);
}

END_SCOPE(objects);
END_NCBI_SCOPE;

