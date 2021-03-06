/* $Id: taxon3.cpp 609817 2020-06-08 15:51:02Z grichenk $
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
 * Author:  Colleen Bollin, based on work by Vladimir Soussov, Michael Domrachev
 *
 * File Description:
 *     NCBI Taxonomy 3 service library implementation
 *
 */

#include <ncbi_pch.hpp>
#include <corelib/ncbistr.hpp>
#include <corelib/ncbienv.hpp>
#include <objects/taxon3/taxon3.hpp>
#include <objects/seqfeat/seqfeat__.hpp>
#include <objects/misc/error_codes.hpp>
#include <connect/ncbi_conn_stream.hpp>
#include <serial/serial.hpp>
#include <serial/enumvalues.hpp>
#include <serial/objistr.hpp>
#include <serial/objostr.hpp>

#include <algorithm>


#define NCBI_USE_ERRCODE_X   Objects_Taxonomy


BEGIN_NCBI_SCOPE
BEGIN_objects_SCOPE // namespace ncbi::objects::


CTaxon3::CTaxon3()
{
    return;
}


CTaxon3::~CTaxon3()
{
}


void
CTaxon3::Init(void)
{
    static const STimeout def_timeout = { 20, 0 };
    CTaxon3::Init(&def_timeout);
}


void
CTaxon3::Init(const STimeout* timeout, unsigned reconnect_attempts)
{
    SetLastError(NULL);

    if ( timeout ) {
        m_timeout_value = *timeout;
        m_timeout = &m_timeout_value;
    } else {
        m_timeout = 0;
    }

    m_nReconnectAttempts = reconnect_attempts;
    
    CNcbiEnvironment env;
    bool bFound = false;
    
    m_sService = env.Get("NI_SERVICE_NAME_TAXON3", &bFound);
    if( !bFound ) {
	m_sService = env.Get("NI_TAXON3_SERVICE_NAME", &bFound);
	if( !bFound ) {
	    m_sService = "TaxService3";
	}
    }

#ifdef USE_TEXT_ASN
    m_eDataFormat = eSerial_AsnText;
#else
    m_eDataFormat = eSerial_AsnBinary;
#endif
}


CRef< CTaxon3_reply >
CTaxon3::SendRequest(const CTaxon3_request& request)
{
    SetLastError(NULL);

	unsigned reconnect_attempts = 0;

	while (reconnect_attempts < m_nReconnectAttempts) {
		try {
			auto_ptr<CObjectOStream> pOut;
			auto_ptr<CObjectIStream> pIn;
			auto_ptr<CConn_ServiceStream>
			    pServer( new CConn_ServiceStream(m_sService, fSERV_Any,
							     0, 0, m_timeout) );

			pOut.reset( CObjectOStream::Open(m_eDataFormat, *pServer) );
			pIn.reset( CObjectIStream::Open(m_eDataFormat, *pServer) );

			CObjectIStream* ppIn = pIn.release();
			CObjectOStream* ppOut = pOut.release();

			try {
				*ppOut << request;
				ppOut->Flush();
				ppOut->Close();

				try {
					CRef< CTaxon3_reply > response(new CTaxon3_reply);
					*ppIn >> *response;

					delete ppIn;
					delete ppOut;

					return response;
							
				} catch (exception& e) {
					SetLastError( e.what() );
				}
			} catch (exception& e) {
				SetLastError( e.what() );
			}

		} catch( exception& e ) {
			SetLastError( e.what() );
		}
		reconnect_attempts++;
	}

	// return NULL
	CRef<CTaxon3_reply> reply;
	return reply;
}

void
CTaxon3::SetLastError( const char* pchErr )
{
    if( pchErr )
        m_sLastError.assign( pchErr );
    else
        m_sLastError.erase();
}


CRef<CTaxon3_reply> CTaxon3::SendOrgRefList(const vector<CRef< COrg_ref> >& list,
					    COrg_ref::fOrgref_parts result_parts,
					    fT3reply_parts t3reply_parts)
{
    CTaxon3_request request;
    if( result_parts != COrg_ref::eOrgref_default ||
	t3reply_parts != eT3reply_default ) {
	CRef<CT3Request> rq(new CT3Request);
	rq->SetJoin().Set().push_back( -result_parts );
        rq->SetJoin().Set().push_back( -t3reply_parts ); // set return parts
	request.SetRequest().push_back(rq);
    }
    ITERATE (vector<CRef< COrg_ref> >, it, list) {
        CRef<CT3Request> rq(new CT3Request);
	CRef<COrg_ref> org(new COrg_ref);
	org->Assign(**it);
	rq->SetOrg(*org);
	request.SetRequest().push_back(rq);
    }
    return SendRequest (request);
}

CRef< CTaxon3_reply >
CTaxon3::SendNameList(const vector<std::string>& list,
		      COrg_ref::fOrgref_parts result_parts,
		      fT3reply_parts t3reply_parts)
{
    CTaxon3_request request;
    if( result_parts != COrg_ref::eOrgref_default ||
	t3reply_parts != eT3reply_default ) {
	CRef<CT3Request> rq(new CT3Request);
	rq->SetJoin().Set().push_back( -result_parts );
        rq->SetJoin().Set().push_back( -t3reply_parts ); // set return parts
	request.SetRequest().push_back(rq);
    }
    ITERATE (vector<std::string>, it, list) {
        CRef<CT3Request> rq(new CT3Request);			
	rq->SetName(*it);
	request.SetRequest().push_back(rq);
    }
    return SendRequest (request);
}

CRef< CTaxon3_reply >
CTaxon3::SendTaxidList(const vector<TTaxId>& list,
		       COrg_ref::fOrgref_parts result_parts,
		       fT3reply_parts t3reply_parts)
{
    CTaxon3_request request;
    if( result_parts != COrg_ref::eOrgref_default ||
	t3reply_parts != eT3reply_default ) {
	CRef<CT3Request> rq(new CT3Request);
	rq->SetJoin().Set().push_back( -result_parts );
        rq->SetJoin().Set().push_back( -t3reply_parts ); // set return parts
	request.SetRequest().push_back(rq);
    }
    ITERATE (vector<TTaxId>, it, list) {
        CRef<CT3Request> rq(new CT3Request);			
	rq->SetTaxid(TAX_ID_TO(int, *it));
	request.SetRequest().push_back(rq);
    }
    return SendRequest (request);
}


END_objects_SCOPE
END_NCBI_SCOPE
