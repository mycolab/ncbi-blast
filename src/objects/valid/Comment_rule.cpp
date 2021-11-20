/* $Id: Comment_rule.cpp 539647 2017-06-26 14:07:48Z bollin $
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
 * Author:  .......
 *
 * File Description:
 *   .......
 *
 * Remark:
 *   This code was originally generated by application DATATOOL
 *   using the following specifications:
 *   'valid.asn'.
 */

// standard includes
#include <ncbi_pch.hpp>

#include <util/static_set.hpp>

#include <objects/misc/sequence_util_macros.hpp>

// generated includes
#include <objects/valid/Comment_rule.hpp>
#include <objects/valid/Field_set.hpp>
#include <objects/valid/Dependent_field_set.hpp>
#include <objects/valid/Dependent_field_rule.hpp>
#include <objects/general/User_field.hpp>
#include <objects/general/Object_id.hpp>
#include <objects/valid/Phrase_list.hpp>
// generated classes

BEGIN_NCBI_SCOPE

BEGIN_objects_SCOPE // namespace ncbi::objects::

// destructor
CComment_rule::~CComment_rule(void)
{
}


void CComment_rule::NormalizePrefix(string& prefix)
{
    if (!NStr::IsBlank(prefix)) {
        while (NStr::StartsWith(prefix, "#")) {
            prefix = prefix.substr(1);
        }
        while (NStr::EndsWith(prefix, "#")) {
            prefix = prefix.substr(0, prefix.length() - 1);
        }
        if (NStr::EndsWith(prefix, "-START", NStr::eNocase)) {
            prefix = prefix.substr(0, prefix.length() - 6);
        } else if (NStr::EndsWith(prefix, "-END", NStr::eNocase)) {
            prefix = prefix.substr(0, prefix.length() - 4);
        }
    }
}


static const char* kStructuredComment       = "StructuredComment";
static const char* kStructuredCommentPrefix = "StructuredCommentPrefix";
static const char* kStructuredCommentSuffix = "StructuredCommentSuffix";

bool CComment_rule::IsStructuredComment (const CUser_object& user)
{
    if (user.IsSetType()
        && user.GetType().IsStr()
        && NStr::EqualNocase(user.GetType().GetStr(),kStructuredComment)) {
        return true;
    } else {
        return false;
    }
}


string CComment_rule::GetStructuredCommentPrefix (const CUser_object& user, bool normalize)
{
    if (!IsStructuredComment(user) || !user.IsSetData()) {
        return "";
    }
    string prefix = "";
    ITERATE(CUser_object::TData, it, user.GetData()) {
        if ((*it)->IsSetData() && (*it)->GetData().IsStr()
            && (*it)->IsSetLabel() && (*it)->GetLabel().IsStr()
            && (NStr::Equal((*it)->GetLabel().GetStr(), kStructuredCommentPrefix)
                || NStr::Equal((*it)->GetLabel().GetStr(), kStructuredCommentSuffix))) {
            prefix = (*it)->GetData().GetStr();
            break;
        }
    }
    if (normalize) {
        CComment_rule::NormalizePrefix(prefix);
    }
    return prefix;
}


string CComment_rule::MakePrefixFromRoot(const string& root)
{
    string prefix = root;
    CComment_rule::NormalizePrefix(prefix);
    prefix = "##" + prefix + "-START##";
    return prefix;
}


string CComment_rule::MakeSuffixFromRoot(const string& root)
{
    string suffix = root;
    CComment_rule::NormalizePrefix(suffix);
    suffix = "##" + suffix + "-END##";
    return suffix;
}


typedef SStaticPair<const char*, const char*> TKeywordPrefix;

const TKeywordPrefix s_StructuredCommentKeywords[] = {
  {"GSC:MIGS:2.1", "MIGS-Data"},
  {"GSC:MIMS:2.1", "MIMS-Data"},
  {"GSC:MIENS:2.1", "MIENS-Data"},
  {"GSC:MIxS;MIGS:3.0", "MIGS:3.0-Data"},
  {"GSC:MIxS;MIMS:3.0", "MIMS:3.0-Data"},
  {"GSC:MIxS;MIMARKS:3.0", "MIMARKS:3.0-Data"} };

static size_t k_NumStructuredCommentKeywords = sizeof(s_StructuredCommentKeywords) / sizeof(TKeywordPrefix);

string CComment_rule::KeywordForPrefix(const string& prefix)
{
    size_t i;
    string compare = prefix;
    CComment_rule::NormalizePrefix(compare);

    for (i = 0; i < k_NumStructuredCommentKeywords; i++) {
        if (NStr::Equal(compare.c_str(), s_StructuredCommentKeywords[i].second)) {
            return s_StructuredCommentKeywords[i].first;
        }
    }
    return "";
}


string CComment_rule::PrefixForKeyword(const string& keyword)
{
    size_t i;

    for (i = 0; i < k_NumStructuredCommentKeywords; i++) {
        if (NStr::Equal(keyword.c_str(), s_StructuredCommentKeywords[i].first)) {
            return s_StructuredCommentKeywords[i].second;
        }
    }
    return "";
}


vector<string> CComment_rule::GetKeywordList()
{
    vector<string> keywords;

    size_t i;

    for (i = 0; i < k_NumStructuredCommentKeywords; i++) {
        keywords.push_back(s_StructuredCommentKeywords[i].first);
    }
    return keywords;
}


CConstRef<CField_rule> CComment_rule::FindFieldRuleRef (const string& field_name) const
{
    ITERATE (CField_set::Tdata, it, GetFields().Get()) {
        CConstRef<CField_rule> p_field_rule = *it;
        if (NStr::Equal(p_field_rule->GetField_name(), field_name)) {
            return p_field_rule;
        }
    }
    return CConstRef<CField_rule>();
}


void CComment_rule::CheckGeneralFieldName(const string& label, TErrorList& errors)
{
    if (NStr::IsBlank(label)) {
        errors.push_back(TError(eSeverity_level_error,
            "Structured Comment contains field without label"));
    }

    if (NStr::Find(label, "::") != string::npos) {
        errors.push_back(TError(eSeverity_level_reject, "Structured comment field '" + label + "' contains double colons"));
    }
}


void CComment_rule::CheckGeneralFieldValue(const string& value, TErrorList& errors)
{
    if (NStr::Find(value, "::") != string::npos) {
        errors.push_back(TError(eSeverity_level_reject, "Structured comment value '" + value + "' contains double colons"));
    }
}


void CComment_rule::CheckGeneralField(const CUser_field& field, TErrorList& errors)
{
    string label = "";

    if (field.IsSetLabel()) {
        if (field.GetLabel().IsStr()) {
            label = field.GetLabel().GetStr();
        } else {
            label = NStr::IntToString(field.GetLabel().GetId());
        }
    }
    CheckGeneralFieldName(label, errors);

    string value = "";
    if (field.GetData().IsStr()) {
        value = (field.GetData().GetStr());
    } else if (field.GetData().IsInt()) {
        value = NStr::IntToString(field.GetData().GetInt());
    }
    CheckGeneralFieldValue(value, errors);
}


void CComment_rule::CheckFieldValue(CConstRef<CField_rule> field_rule, const string& value, TErrorList& errors) const
{
    if (field_rule) {
        if (!field_rule->DoesStringMatchRuleExpression(value)) {
            // post error about not matching format
            CField_rule::TSeverity sev = field_rule->GetSeverity();
            if (NStr::EqualNocase(field_rule->GetField_name(), "Finishing Goal")
                && NStr::EqualNocase(GetPrefix(), "##Genome-Assembly-Data-START##")) {
                sev = eSeverity_level_error;
            } else if (NStr::EqualNocase(field_rule->GetField_name(), "Current Finishing Status")
                && NStr::EqualNocase(GetPrefix(), "##Genome-Assembly-Data-START##")) {
                sev = eSeverity_level_error;
            }
            errors.push_back(TError(sev, value + " is not a valid value for " + field_rule->GetField_name()));
        }
    }
    if (IsSetForbidden_phrases()) {
        ITERATE(CComment_rule::TForbidden_phrases::Tdata, it, GetForbidden_phrases().Get()) {
            if (NStr::FindNoCase(value, *it) != string::npos) {
                errors.push_back(TError(eSeverity_level_error, "'" + value + "' is inappropriate for a GenBank submisison"));
            }
        }
    }
    CheckGeneralFieldValue(value, errors);
}


void CComment_rule::CheckFieldValue(CConstRef< CField_rule> rule, const CUser_field& field, TErrorList& errors) const
{
    string value = "";
    if (field.GetData().IsStr()) {
        value = (field.GetData().GetStr());
    } else if (field.GetData().IsInt()) {
        value = NStr::IntToString(field.GetData().GetInt());
    }
    CheckFieldValue(rule, value, errors);
}


CComment_rule::TErrorList CComment_rule::IsValid(const CUser_object& user) const
{
    TErrorList errors;

    CField_set::Tdata::const_iterator field_rule = GetFields().Get().begin();
    CUser_object::TData::const_iterator field = user.GetData().begin();
    while (field_rule != GetFields().Get().end()
           && field != user.GetData().end()) {
        string encountered_field = kEmptyStr;
        if ((*field)->IsSetLabel()) {
            if ((*field)->GetLabel().IsStr()) {
                encountered_field = (*field)->GetLabel().GetStr();
            } else {
                encountered_field = NStr::IntToString((*field)->GetLabel().GetId());
            }
        }
            // skip suffix and prefix
        if (NStr::Equal(encountered_field, "StructuredCommentPrefix")
            || NStr::Equal(encountered_field, "StructuredCommentSuffix")) {
            ++field;
            continue;
        } else if (NStr::IsBlank(encountered_field)) {
            CheckGeneralField(**field, errors);
            ++field;
        }

        const string & expected_field = (*field_rule)->GetField_name();
        if (NStr::Equal(expected_field, encountered_field)) {
            // field in correct order
            // is value correct?
            CheckFieldValue(*field_rule, **field, errors);
            ++field;
            ++field_rule;
        } else {
            // find field for this rule and validate it
            CConstRef<CUser_field> p_other_field = user.GetFieldRef(expected_field);
            if( ! p_other_field ) {
                if ((*field_rule)->IsSetRequired() && (*field_rule)->GetRequired()) {
                    errors.push_back(TError((*field_rule)->GetSeverity(),
                                            "Required field " + (*field_rule)->GetField_name() + " is missing"));
                }
            } else {
                const CUser_field& other_field = *p_other_field;
                if (GetRequire_order()) {
                    errors.push_back(TError((*field_rule)->GetSeverity(),
                                            expected_field + " field is out of order"));
                }
                CheckFieldValue(*field_rule, other_field, errors);

            }
            ++field_rule;

            // find rule for this field
            CConstRef<CField_rule> real_field_rule = FindFieldRuleRef(encountered_field);
            if (!real_field_rule) {
                if (!IsSetAllow_unlisted()) {
                    // field not found, not legitimate field name
                    errors.push_back(TError(eSeverity_level_error,
                        encountered_field + " is not a valid field name"));
                }
                CheckGeneralField(**field, errors);
                ++field;
            }
        }
    }

    while (field_rule != GetFields().Get().end()) {
        if ((*field_rule)->IsSetRequired() && (*field_rule)->GetRequired()) {
            errors.push_back(TError((*field_rule)->GetSeverity(),
                        "Required field " + (*field_rule)->GetField_name() + " is missing"));
        }
        ++field_rule;
    }

    while (field != user.GetData().end()) {
        // skip suffix and prefix
        if ((*field)->IsSetLabel()) {
            string label = "";
            if ((*field)->GetLabel().IsStr()) {
                label = (*field)->GetLabel().GetStr();
            } else {
                label = NStr::IntToString((*field)->GetLabel().GetId());
            }
            if (NStr::Equal(label, "StructuredCommentPrefix")
                || NStr::Equal(label, "StructuredCommentSuffix")) {
                ++field;
                continue;
            }

            CConstRef<CField_rule> p_field_rule = FindFieldRuleRef(label);
            CConstRef<CUser_field> p_other_field;
            if( p_field_rule ) {
                p_other_field = user.GetFieldRef(label);
            }
            if( p_field_rule && p_other_field ) {
                if (p_other_field.GetPointer() != (*field)) {
                    errors.push_back(TError(p_field_rule->GetSeverity(),
                                            "Multiple values for " + label + " field"));
                }
            } else {
                if (!IsSetAllow_unlisted()) {
                    // field not found, not legitimate field name
                    errors.push_back(TError(eSeverity_level_error,
                                            label + " is not a valid field name"));
                }
                CheckGeneralField(**field, errors);
            }
        } else {
            CheckGeneralField(**field, errors);
        }
        ++field;
    }

    // now look at dependent rules
    if (IsSetDependent_rules()) {
        ITERATE (CDependent_field_set::Tdata, depend_rule, GetDependent_rules().Get()) {
            if( ! (*depend_rule)->IsSetMatch_name() ) {
                continue;
            }
            const string & depend_field_name = (*depend_rule)->GetMatch_name();
            CConstRef<CUser_field> p_depend_field = user.GetFieldRef(depend_field_name);
            if( ! p_depend_field ) {
                continue;
            }
            string value = "";
            if (p_depend_field->GetData().IsStr()) {
                value = (p_depend_field->GetData().GetStr());
            } else if (p_depend_field->GetData().IsInt()) {
                value = NStr::IntToString(p_depend_field->GetData().GetInt());
            }

            bool is_invert_match = (*depend_rule)->IsSetInvert_match() && (*depend_rule)->GetInvert_match();
            bool does_match_rule_expression = (*depend_rule)->DoesStringMatchRuleExpression(value);
            if ( is_invert_match != does_match_rule_expression) {
                // other rules apply
                if ((*depend_rule)->IsSetOther_fields()) {
                    ITERATE (CField_set::Tdata, other_rule, (*depend_rule)->GetOther_fields().Get()) {

                        const string & other_field_name = (*other_rule)->GetField_name();
                        CConstRef<CUser_field> p_other_field = user.GetFieldRef(other_field_name);
                        if( ! p_other_field ) {
                            // unable to find field
                            if ((*other_rule)->IsSetRequired() && (*other_rule)->GetRequired()) {
                                errors.push_back(TError((*other_rule)->GetSeverity(),
                                                        "Required field " + (*other_rule)->GetField_name() + " is missing when "
                                                        + depend_field_name + " has value '" + value + "'"));
                            }
                            continue;
                        }

                        string other_value = "";
                        if (p_other_field->GetData().IsStr()) {
                            other_value = (p_other_field->GetData().GetStr());
                        } else if (p_other_field->GetData().IsInt()) {
                            other_value = NStr::IntToString(p_other_field->GetData().GetInt());
                        }
                        if (!(*other_rule)->DoesStringMatchRuleExpression(other_value)) {
                            // post error about not matching format
                            errors.push_back(TError((*other_rule)->GetSeverity(),
                                                    other_value + " is not a valid value for " + other_field_name
                                                    + " when " + depend_field_name + " has value '" + value + "'"));
                        }
                    }
                }
                if ((*depend_rule)->IsSetDisallowed_fields()) {
                    ITERATE (CField_set::Tdata, other_rule, (*depend_rule)->GetDisallowed_fields().Get()) {
                        if( (*other_rule)->IsSetField_name() ) {
                            const string & other_field_name = (*other_rule)->GetField_name();
                            // found field that should not be present
                            errors.push_back(TError((*other_rule)->GetSeverity(),
                                                    other_field_name + " is not a valid field name when " + depend_field_name + " has value '" + value + "'"));
                        }
                    }
                }
            }
        }
    }

    return errors;
}


bool CComment_rule::ReorderFields(CUser_object& user) const
{
    if (!IsSetFields() || !user.IsSetData()) {
        return false;
    }
    bool any_change = false;
    CField_set::Tdata::const_iterator field_rule = GetFields().Get().begin();
    CUser_object::TData::iterator unchecked_fields = user.SetData().begin();
    while (field_rule != GetFields().Get().end() && unchecked_fields != user.SetData().end()) {
        CUser_object::TData::iterator field = unchecked_fields;
        while (field != user.SetData().end()) {
            if ((*field)->IsSetLabel()
                && (*field)->GetLabel().IsStr()) {
                string label = (*field)->GetLabel().GetStr();
                if (NStr::Equal(label, kStructuredCommentPrefix)) {
                    // push to beginning if not already there
                    if (field == user.SetData().begin()) {
                        // already in position, just increment
                        ++unchecked_fields;
                        ++field;
                    } else {
                        CRef<CUser_field> new_field(new CUser_field());
                        new_field->Assign(**field);
                        user.SetData().erase(field);
                        user.SetData().insert(user.SetData().begin(), new_field);
                        unchecked_fields = user.SetData().begin();
                        unchecked_fields++;
                        field = unchecked_fields;
                        field_rule = GetFields().Get().begin();
                        any_change = true;
                    }
                } else if (NStr::Equal(label, kStructuredCommentSuffix)) {
                    // TODO: move to end if not already there
                    // for now just skip
                    if (*field == user.SetData().back()) {
                        // already at the end, just skip
                        ++field;
                    } else {
                        CRef<CUser_field> new_field(new CUser_field());
                        new_field->Assign(**field);
                        user.SetData().erase(field);
                        user.SetData().push_back(new_field);
                        unchecked_fields = user.SetData().begin();
                        field = unchecked_fields;
                        field_rule = GetFields().Get().begin();
                        any_change = true;
                    }
                } else if (NStr::Equal(label, (*field_rule)->GetField_name())) {
                    if (field == unchecked_fields) {
                        // already in position, just increment
                        ++unchecked_fields;
                        ++field;
                    } else {
                        CRef<CUser_field> new_field(new CUser_field());
                        new_field->Assign(**field);
                        user.SetData().erase(field);
                        user.SetData().insert(unchecked_fields, new_field);
                        unchecked_fields = user.SetData().begin();
                        field = unchecked_fields;
                        field_rule = GetFields().Get().begin();
                        any_change = true;
                    }
                } else {
                    ++field;
                }
            } else {
                ++field;
            }
        }
        ++field_rule;
    }
    return any_change;
}


CComment_rule::TErrorList CComment_rule::CheckGeneralStructuredComment(const CUser_object& user)
{
    TErrorList errors;

    if (!user.IsSetData()) {
        return errors;
    }

    ITERATE(CUser_object::TData, it, user.GetData()) {
        CheckGeneralField(**it, errors);
    }
    return errors;
}


END_objects_SCOPE // namespace ncbi::objects::

END_NCBI_SCOPE

/* Original file checksum: lines: 57, chars: 1729, CRC32: 46a495ac */
