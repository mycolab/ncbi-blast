#ifndef CORELIB___REQUEST_CONTROL__HPP
#define CORELIB___REQUEST_CONTROL__HPP

/*  $Id: request_control.hpp 574926 2018-11-20 20:23:54Z ucko $
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
 * Authors:  Denis Vakatov, Vladimir Ivanov, Victor Joukov
 *
 * File Description:
 *   Manage request rate to some shared resource
 *
 */

#include <corelib/ncbi_limits.hpp>
#include <corelib/ncbitime.hpp>
#include <deque>

/** @addtogroup Utility
 *
 * @{
 */

BEGIN_NCBI_SCOPE


/////////////////////////////////////////////////////////////////////////////
///
/// CRequestRateControlException --
///
/// Define exceptions generated by CRequestThrottler.
///
/// CRequestThrottlerException inherits its basic functionality from
/// CCoreException and defines additional error codes.

class NCBI_XNCBI_EXPORT CRequestRateControlException : public CCoreException
{
public:
    /// Error types that CRequestRateControl can generate.
    enum EErrCode {
        eNumRequestsMax,         ///< Maximum number of requests exceeded;
        eNumRequestsPerPeriod,   ///< Number of requests per period exceeded;
        eMinTimeBetweenRequests  ///< The time between two consecutive requests
                                 ///< is too short;
    };
    /// Translate from the error code value to its string representation.
    virtual const char* GetErrCodeString(void) const override;

    // Standard exception boilerplate code.
    NCBI_EXCEPTION_DEFAULT(CRequestRateControlException, CCoreException);
};



/////////////////////////////////////////////////////////////////////////////
///
/// CRequestRateControl --
///
/// Manage request rate to some shared resource, for example.

class NCBI_XNCBI_EXPORT CRequestRateControl
{
public:
    /// Special value for maximum number of allowed requests per period.
    /// Disable any kind of request throttling.
    /// 
    /// @sa 
    ///   Reset
    static const unsigned int kNoLimit = kMax_UInt;

    /// What to do if exceeded the rate limits.
    enum EThrottleAction {
        eSleep,      ///< Sleep till the rate requirements are met & return
        eErrCode,    ///< Return immediately with err code == FALSE
        eException,  ///< Throw an exception
        eDefault     ///< in c-tor -- eSleep;  in Approve() -- value set in c-tor
    };

    /// Throttle mode.
    ///
    /// In case if number of requests and time period are specified,
    /// it is possible to select between two modes for request throttler.
    /// First mode is eContinuous. It use internal time line to check number
    /// of requests in the past period of time, using current time as ending
    /// point for that period. Starting point determinates with ordinary
    /// subtraction of "per_period" time, specified in object's constructor,
    /// from current time. So the controlled time frame moves continuously
    /// in time.
    /// Contrary to continuos mode, eDiscrete mode have fixed starting point
    /// for period of time, where throttler checks number of incoming
    /// requests. First time period starts when CRequestRateControl object
    /// creates. Each next period starts with interval of "per_period",
    /// or from first approved request in case of long period of inactivity.
    /// When each new period starts, the throttler drops all restrictions,
    /// and starts to count number of allowed requests per period from zero.
    /// Usually eDiscrete mode is a little bit faster and less memory consuming.
    enum EThrottleMode {
        eContinuous, ///< Uses float time frame to check number of requests
        eDiscrete    ///< Uses fixed time frame to check number of requests
    };

    /// Constructor.
    ///
    /// Construct class object. Run Reset() method.
    ///
    /// @sa 
    ///   Reset, EThrottleAction, EThrottleMode
    CRequestRateControl
              (unsigned int     num_requests_allowed,
               CTimeSpan        per_period                = CTimeSpan(1,0),
               CTimeSpan        min_time_between_requests = CTimeSpan(0,0),
               EThrottleAction  throttle_action           = eDefault,
               EThrottleMode    throttle_mode             = eContinuous);

    /// Set new restriction for throttling mechanism.
    ///
    /// Zero values for time spans 'per_period' or 'min_time_between_requests'
    /// means no rate restriction for that throttling mechanism, respectively.
    ///
    /// @param num_requests_allowed
    ///   Maximum number of allowed requests per 'per_period'.
    ///   Can be kNoLimit for unlimited number of requests (throttler is disabled,
    ///   Approve() always returns TRUE).
    /// @param per_period
    ///   Time span in which only 'num_requests_allowed' requests can be
    ///   approved.
    /// @param min_time_between_requests
    ///   Minimum time between two succesful consecutive requests.
    /// @param throttle_action
    ///   Set throttle action by default. The eDefault means eSleep here.
    /// @param throttle_mode
    ///   Set throttle action by default. The eDefault means eSleep here.
    /// For backward compatibility, use eContinuous mode by default.
    /// @sa
    ///   Approve, ApproveTime
    void Reset(unsigned int     num_requests_allowed,
               CTimeSpan        per_period                = CTimeSpan(1,0),
               CTimeSpan        min_time_between_requests = CTimeSpan(0,0),
               EThrottleAction  throttle_action           = eDefault,
               EThrottleMode    throttle_mode             = eContinuous);

    /// Approve a request.
    ///
    /// @param action
    ///   Throttle action used by this function call. If passed argument
    ///   equal to eDefault that use throttle action was set in 
    ///   the constructor.
    /// @return
    ///   Return TRUE if everything meet to established requirements.
    ///   Return FALSE if some requirements are not passed, or
    ///   throw exception if throttle action was set to eException.
    /// @sa
    ///   Reset, ApproveTime
    bool Approve(EThrottleAction action = eDefault);

    /// Get a time span in which request can be approved.
    ///
    /// You should call this method until it returns zero time span, otherwise
    /// you should sleep (using Sleep() method) for indicated time.
    ///
    /// @return
    ///   Returns time to wait until actual request, zero if can proceed
    ///   immediately.
    ///   If you use this method with absolute limitation (no time period and
    ///   no minimum between requests) and the limitation is exhausted it will
    ///   throw an exception.
    /// @sa
    ///   Reset, Approve
    CTimeSpan ApproveTime(void);

    /// Sleep for CTimeSpan.
    ///
    /// @param sleep_time
    ///   For how long to sleep. If it's impossible to sleep to that long in
    ///   millisecond range, rounds up sleep time to the whole seconds.
    static void Sleep(CTimeSpan sleep_time);

    /// Lock/unlock functions for use by generic RAII guard CGuard.
    /// See 'corelib/guard.hpp' for details.
    
    void Lock()   { Approve(eSleep); }
    void Unlock() { /* do nothing */ }

    /// Check if throttling is enabled.
    bool IsEnabled(void) const { return m_NumRequestsAllowed != kNoLimit; }

private:
    typedef double TTime;

    ///
    bool x_Approve(EThrottleAction action, CTimeSpan *sleeptime);

    /// Remove from the list of approved requests all expared items.
    void x_CleanTimeLine(TTime now);

private:
    // Saved parameters
    EThrottleMode    m_Mode;
    unsigned int     m_NumRequestsAllowed;
    TTime            m_PerPeriod;
    TTime            m_MinTimeBetweenRequests;
    EThrottleAction  m_ThrottleAction;

    CStopWatch       m_StopWatch;      ///< Stopwatch to measure elapsed time
    typedef deque<TTime> TTimeLine;
    TTimeLine        m_TimeLine;       ///< Vector of times of approvals
    TTime            m_LastApproved;   ///< Last approve time
    unsigned int     m_NumRequests;    ///< Num requests per period
};


//////////////////////////////////////////////////////////////////////////////
//
// Inline
//

inline
bool CRequestRateControl::Approve(EThrottleAction action)
{
    return x_Approve(action, 0);
}

inline
CTimeSpan CRequestRateControl::ApproveTime()
{
    CTimeSpan sleeptime;
    bool res = x_Approve(eSleep, &sleeptime);
    if ( !res ) {
        return sleeptime;
    }
    // Approve request
    return CTimeSpan(0, 0);
}


END_NCBI_SCOPE

/* @} */

#endif  /* CORELIB___REQUEST_CONTROL__HPP */
