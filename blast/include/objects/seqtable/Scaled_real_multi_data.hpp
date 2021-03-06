/* $Id: Scaled_real_multi_data.hpp 457991 2015-01-29 19:26:03Z vasilche $
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
 */

/// @file Scaled_real_multi_data.hpp
/// User-defined methods of the data storage class.
///
/// This file was originally generated by application DATATOOL
/// using the following specifications:
/// 'seqtable.asn'.
///
/// New methods or data members can be added to it if needed.
/// See also: Scaled_real_multi_data_.hpp


#ifndef OBJECTS_SEQTABLE_SCALED_REAL_MULTI_DATA_HPP
#define OBJECTS_SEQTABLE_SCALED_REAL_MULTI_DATA_HPP


// generated includes
#include <objects/seqtable/Scaled_real_multi_data_.hpp>

// generated classes

BEGIN_NCBI_SCOPE

BEGIN_objects_SCOPE // namespace ncbi::objects::

/////////////////////////////////////////////////////////////////////////////
class NCBI_SEQ_EXPORT CScaled_real_multi_data : public CScaled_real_multi_data_Base
{
    typedef CScaled_real_multi_data_Base Tparent;
public:
    // constructor
    CScaled_real_multi_data(void);
    // destructor
    ~CScaled_real_multi_data(void);

    bool TryGetReal(size_t row, double& v) const;

private:
    // Prohibit copy constructor and assignment operator
    CScaled_real_multi_data(const CScaled_real_multi_data& value);
    CScaled_real_multi_data& operator=(const CScaled_real_multi_data& value);

};

/////////////////// CScaled_real_multi_data inline methods

// constructor
inline
CScaled_real_multi_data::CScaled_real_multi_data(void)
{
}


/////////////////// end of CScaled_real_multi_data inline methods


END_objects_SCOPE // namespace ncbi::objects::

END_NCBI_SCOPE


#endif // OBJECTS_SEQTABLE_SCALED_REAL_MULTI_DATA_HPP
/* Original file checksum: lines: 86, chars: 2664, CRC32: c456928b */
