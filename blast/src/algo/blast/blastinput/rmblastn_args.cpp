/*  $Id: rmblastn_args.cpp 500404 2016-05-04 14:59:01Z camacho $
 * ===========================================================================
 *
 *                            PUBLIC DOMAIN NOTICE
 *
 * ===========================================================================
 *
 * Author: Robert M. Hubley
 *         Shameless based on the work of Christiam Camacho in 
 *           rmblastn_args.cpp
 *
 */

/** @file rmblastn_args.cpp
 * Implementation of the RMBLASTN command line arguments
 */


#include <ncbi_pch.hpp>
#include <algo/blast/blastinput/rmblastn_args.hpp>
#include <algo/blast/api/disc_nucl_options.hpp>
#include <algo/blast/api/blast_exception.hpp>
#include <algo/blast/blastinput/blast_input_aux.hpp>

BEGIN_NCBI_SCOPE
BEGIN_SCOPE(blast)
USING_SCOPE(objects);

CRMBlastnAppArgs::CRMBlastnAppArgs()
{
    CRef<IBlastCmdLineArgs> arg;
    // Something appears to be overriding this
    arg.Reset(new CProgramDescriptionArgs("rmblastn", 
                                          "Nucleotide-Nucleotide BLAST with RepeatMasker Extensions"));
    const bool kQueryIsProtein = false;
    m_Args.push_back(arg);

    // Don't want the greedy megablast...no howdy doody
    static const string kDefaultTask = "rmblastn";
    SetTask(kDefaultTask);
    set<string> tasks
        (CBlastOptionsFactory::GetTasks(CBlastOptionsFactory::eNuclNucl));
    arg.Reset(new CTaskCmdLineArgs(tasks, kDefaultTask));
    m_Args.push_back(arg);

    m_BlastDbArgs.Reset(new CBlastDatabaseArgs);
    arg.Reset(m_BlastDbArgs);
    m_Args.push_back(arg);

    m_StdCmdLineArgs.Reset(new CStdCmdLineArgs);
    arg.Reset(m_StdCmdLineArgs);
    m_Args.push_back(arg);

    arg.Reset(new CGenericSearchArgs(kQueryIsProtein, false, true));
    m_Args.push_back(arg);

    arg.Reset(new CNuclArgs);
    m_Args.push_back(arg);

    arg.Reset(new CDiscontiguousMegablastArgs);
    m_Args.push_back(arg);

    arg.Reset(new CFilteringArgs(kQueryIsProtein));
    m_Args.push_back(arg);
  
    // RMH: Wasn't previously an option in blastn
    arg.Reset(new CRMBlastNArg );
    m_Args.push_back(arg);

    arg.Reset(new CGappedArgs);
    m_Args.push_back(arg);

    m_HspFilteringArgs.Reset(new CHspFilteringArgs);
    arg.Reset(m_HspFilteringArgs);
    m_Args.push_back(arg);

    arg.Reset(new CWindowSizeArg);
    m_Args.push_back(arg);

    arg.Reset(new CMbIndexArgs);
    m_Args.push_back(arg);

    m_QueryOptsArgs.Reset(new CQueryOptionsArgs(kQueryIsProtein));
    arg.Reset(m_QueryOptsArgs);
    m_Args.push_back(arg);

    m_FormattingArgs.Reset(new CFormattingArgs);
    arg.Reset(m_FormattingArgs);
    m_Args.push_back(arg);

    m_MTArgs.Reset(new CMTArgs);
    arg.Reset(m_MTArgs);
    m_Args.push_back(arg);

    m_RemoteArgs.Reset(new CRemoteArgs);
    arg.Reset(m_RemoteArgs);
    m_Args.push_back(arg);

    m_DebugArgs.Reset(new CDebugArgs);
    arg.Reset(m_DebugArgs);
    m_Args.push_back(arg);
}

CRef<CBlastOptionsHandle> 
CRMBlastnAppArgs::x_CreateOptionsHandle(CBlastOptions::EAPILocality locality,
                                      const CArgs& args)
{
    CRef<CBlastOptionsHandle> retval =
        x_CreateOptionsHandleWithTask(locality, args[kTask].AsString());
    return retval;
}

int
CRMBlastnAppArgs::GetQueryBatchSize() const
{
    return blast::GetQueryBatchSize(ProgramNameToEnum(GetTask()));
}

END_SCOPE(blast)
END_NCBI_SCOPE

