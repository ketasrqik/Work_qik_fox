/*
 *  Compilation: gcc -Wall ex1.c -o ex1 -lclamav
 *
 *  Copyright (C) 2013-2022 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 *  Copyright (C) 2007-2013 Sourcefire, Inc.
 *  Author: Tomasz Kojm <tkojm@clamav.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA 02110-1301, USA.
 */

//#if HAVE_CONFIG_H
//#include "clamav-config.h"
//#endif
 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>                                     
#include <clamav.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

// libclamav
#include "clamav.h"
#include "others.h" 
#include "str.h" 

// common
#include "optparser.h"
#include "output.h"
#include "misc.h" 

// libfreshclam
#include "libfreshclam.h" 
#include "execute.h"
#include "notify.h" 

//int g_sigchildWait                      = 1;
//short g_terminate                       = 0;
//short g_foreground                      = -1;
//const char *g_pidfile                   = NULL;
//char g_freshclamTempDirectory[PATH_MAX] = {0};

//typedef struct fc_ctx_ {
//    uint32_t bTestDatabases;
//    uint32_t bBytecodeEnabled;
//} fc_ctx; 

//static void libclamav_msg_callback(enum cl_msg severity, const char *fullmsg, const char *msg, void *ctx)
//{
//    UNUSEDPARAM(fullmsg);
//    UNUSEDPARAM(ctx);
//
//    switch (severity) {
//        case CL_MSG_ERROR:
//            logg("^[LibClamAV] %s", msg);
//            break;
//        case CL_MSG_WARN:
//            logg("~[LibClamAV] %s", msg);
//            break;
//        default:
//            logg("*[LibClamAV] %s", msg);
//            break;
//    }
//} 
//
//static void libclamav_msg_callback_quiet(enum cl_msg severity, const char *fullmsg, const char *msg, void *ctx)
//{
//    UNUSEDPARAM(fullmsg);
//    UNUSEDPARAM(ctx);
//
//    switch (severity) {
//        case CL_MSG_ERROR:
//            logg("^[LibClamAV] %s", msg);
//            break;
//        default:
//            break;
//    }
//} 
//
//fc_error_t download_complete_callback(const char *dbFilename, void *context)
//{
//    fc_error_t status = FC_EARG;
//    fc_error_t ret;
//    fc_ctx *fc_context = (fc_ctx *)context;
//
//#ifndef _WIN32
//    char firstline[256];
//    char lastline[256];
//    int pipefd[2];
//    pid_t pid;
//    int stat_loc = 0;
//    int waitpidret;
//#endif
//
//    if ((NULL == context) || (NULL == dbFilename)) {
//        logg("^Invalid arguments to download_complete_callback.\n");
//        goto done;
//    }
//
//    logg("*download_complete_callback: Download complete for database : %s\n", dbFilename);
//    logg("*download_complete_callback:   fc_context->bTestDatabases   : %u\n", fc_context->bTestDatabases);
//    logg("*download_complete_callback:   fc_context->bBytecodeEnabled : %u\n", fc_context->bBytecodeEnabled);
//
//    logg("Testing database: '%s' ...\n", dbFilename);
//
//    if (fc_context->bTestDatabases) {
//#ifdef _WIN32
//
//        __try {
//            ret = fc_test_database(dbFilename, fc_context->bBytecodeEnabled);
//        } __except (logg("!Exception during database testing, code %08x\n",
//                         GetExceptionCode()),
//                    EXCEPTION_CONTINUE_SEARCH) {
//            ret = FC_ETESTFAIL;
//        }
//        if (FC_SUCCESS != ret) {
//            logg("^Database load exited with \"%s\"\n", fc_strerror(ret));
//            status = FC_ETESTFAIL;
//            goto done;
//        }
//
//#else
//
//        if (pipe(pipefd) == -1) {
//            /*
//             * Failed to create pipe.
//             * Test database without using pipe & child process. 
//             */
//            logg("^pipe() failed: %s\n", strerror(errno));
//            ret = fc_test_database(dbFilename, fc_context->bBytecodeEnabled);
//            if (FC_SUCCESS != ret) {
//                logg("^Database load exited with \"%s\"\n", fc_strerror(ret));
//                status = FC_ETESTFAIL;
//                goto done;
//            }
//        } else {
//            /*
//             * Attempt to test database in a child process.
//             */
//
//            /* We need to be able to wait for the child process ourselves.
//             * We'll re-enable wait in the global handler when we're done. */
//            g_sigchildWait = 0;
//
//            switch (pid = fork()) {
//                case -1: {
//                    /*
//                     * Fork failed.
//                     * Test database without using pipe & child process.
//                     */
//                    close(pipefd[0]);
//                    close(pipefd[1]);
//                    logg("^fork() to test database failed: %s\n", strerror(errno));
//
//                    /* Test the database without forking. */
//                    ret = fc_test_database(dbFilename, fc_context->bBytecodeEnabled);
//                    if (FC_SUCCESS != ret) {
//                        logg("^Database load exited with \"%s\"\n", fc_strerror(ret));
//                        status = FC_ETESTFAIL;
//                        goto done;
//                    }
//                    break;
//                }
//                case 0: {
//                    /*
//                     * Child process.
//                     */
//                    close(pipefd[0]);
//
//                    /* Redirect stderr to the pipe for the parent process */
//                    if (dup2(pipefd[1], 2) == -1) {
//                        logg("^dup2() call to redirect stderr to pipe failed: %s\n", strerror(errno));
//                    }
//
//                    /* Test the database */
//                    status = fc_test_database(dbFilename, fc_context->bBytecodeEnabled);
//                    exit(status);
//                }
//                default: {
//                    /*
//                     * Original/parent process.
//                     */
//                    FILE *pipeHandle = NULL;
//
//                    /* read first / last line printed by child */
//                    close(pipefd[1]);
//                    pipeHandle   = fdopen(pipefd[0], "r");
//                    firstline[0] = 0;
//                    lastline[0]  = 0;
//                    do {
//                        if (!fgets(firstline, sizeof(firstline), pipeHandle))
//                            break;
//                        /* ignore warning messages, otherwise the outdated warning will
//                         * make us miss the important part of the error message */
//                    } while (!strncmp(firstline, "LibClamAV Warning:", 18));
//                    /* must read entire output, child doesn't like EPIPE */
//                    while (fgets(lastline, sizeof(firstline), pipeHandle)) {
//                        /* print the full output only when LogVerbose or -v is given */
//                        logg("*%s", lastline);
//                    }
//                    fclose(pipeHandle);
//                    pipeHandle = NULL;
//
//                    while ((-1 == (waitpidret = waitpid(pid, &stat_loc, 0))) && (errno == EINTR)) {
//                        continue;
//                    }
//
//                    if ((waitpidret == -1) && (errno != ECHILD))
//                        logg("^waitpid() failed: %s\n", strerror(errno));
//
//                    /* Strip trailing whitespace from child error output */
//                    cli_chomp(firstline);
//                    cli_chomp(lastline);
//
//                    if (firstline[0]) {
//                        /* The child process output some error messages */
//                        logg("^Stderr output from database load : %s%s%s\n", firstline, lastline[0] ? " [...] " : "", lastline);
//                    }
//
//                    if (WIFEXITED(stat_loc)) {
//                        ret = (fc_error_t)WEXITSTATUS(stat_loc);
//                        if (FC_SUCCESS != ret) {
//                            logg("^Database load exited with \"%s\"\n", fc_strerror(ret));
//                            status = FC_ETESTFAIL;
//                            goto done;
//                        }
//
//                        if (firstline[0])
//                            logg("^Database successfully loaded, but there is stderr output\n");
//
//                    } else if (WIFSIGNALED(stat_loc)) {
//                        logg("!Database load killed by signal %d\n", WTERMSIG(stat_loc));
//                        status = FC_ETESTFAIL;
//                        goto done;
//                    } else {
//                        logg("^Unknown status from wait: %d\n", stat_loc);
//                        status = FC_ETESTFAIL;
//                        goto done;
//                    }
//                }
//            }
//        }
//#endif
//    }
//
//    status = FC_SUCCESS;
//
//done:
//
//    if (FC_SUCCESS == status) {
//        logg("Database test passed.\n");
//    } else {
//        logg("!Database test FAILED.\n");
//    }
//
//    /* Re-enable the global handler's child process wait */
//    g_sigchildWait = 1;
//
//    return status;
//}
//
//static fc_error_t get_server_node(
//    const char *server,
//    char *defaultProtocol,
//    char **serverUrl)
//{
//    fc_error_t status = FC_EARG;
//
//    char *url     = NULL;
//    size_t urlLen = 0;
//
//    if ((NULL == server) || (NULL == defaultProtocol) || (NULL == serverUrl)) {
//        mprintf("!get_server_node: Invalid args!\n");
//        goto done;
//    }
//
//    *serverUrl = NULL;
//
//    /*
//     * Ensure that URL contains protocol.
//     */
//    if (!strncmp(server, "db.", 3) && strstr(server, ".clamav.net")) {
//        url = cli_strdup("https://database.clamav.net");
//        if (NULL == url) {
//            logg("!get_server_node: Failed to duplicate string for database.clamav.net url.\n");
//            status = FC_EMEM;
//            goto done;
//        }
//    } else if (!strstr(server, "://")) {
//        urlLen = strlen(defaultProtocol) + strlen("://") + strlen(server);
//        url    = malloc(urlLen + 1);
//        if (NULL == url) {
//            logg("!get_server_node: Failed to allocate memory for server url.\n");
//            status = FC_EMEM;
//            goto done;
//        }
//        snprintf(url, urlLen + 1, "%s://%s", defaultProtocol, server);
//    } else {
//        urlLen = strlen(server);
//        url    = cli_strdup(server);
//        if (NULL == url) {
//            logg("!get_server_node: Failed to duplicate string for server url.\n");
//            status = FC_EMEM;
//            goto done;
//        }
//    }
//
//    *serverUrl = url;
//    status     = FC_SUCCESS;
//
//done:
//    return status;
//}
//
//static fc_error_t string_list_add(const char *item, char ***stringList, uint32_t *nListItems)
//{
//    fc_error_t status = FC_EARG;
//
//    char **newList  = NULL;
//    uint32_t nItems = 0;
//
//    if ((NULL == item) || (NULL == stringList) || (NULL == nListItems)) {
//        mprintf("!string_list_add: Invalid arguments.\n");
//        goto done;
//    }
//
//    nItems  = *nListItems + 1;
//    newList = (char **)cli_realloc(*stringList, nItems * sizeof(char *));
//    if (newList == NULL) {
//        mprintf("!string_list_add: Failed to allocate memory for optional database list entry.\n");
//        status = FC_EMEM; 
//        goto done;
//    }      
//
//    *stringList = newList;
//
//    newList[nItems - 1] = cli_strdup(item);
//    if (newList[nItems - 1] == NULL) {
//        mprintf("!string_list_add: Failed to allocate memory for optional database list item.\n");
//        status = FC_EMEM;
//        goto done;
//    }
//
//    *nListItems = nItems;
//    status      = FC_SUCCESS;
//
//done:
//
//    return status;
//}
//
//static void free_string_list(char **stringList, uint32_t nListItems)
//{
//    uint32_t i;
//
//    if (NULL != stringList) {
//        for (i = 0; i < nListItems; i++) {
//            if (stringList[i] != NULL) {
//                free(stringList[i]);
//                stringList[i] = NULL;
//            }
//        }
//
//        free(stringList);
//    }
//} 
//
//static fc_error_t get_database_server_list(
//    struct optstruct *opts,
//    char ***serverList,
//    uint32_t *nServers,
//    int *bPrivate)
//{
//    fc_error_t ret;
//    fc_error_t status = FC_EARG;
//    const struct optstruct *opt;
//    char **servers      = NULL;
//    uint32_t numServers = 0;
//
//    if ((NULL == opts) || (NULL == serverList) || (NULL == nServers) || (NULL == bPrivate)) {
//        mprintf("!get_database_server_list: Invalid args!\n");
//        goto done;
//    }
//
//    *serverList = NULL;
//    *nServers   = 0;
//    *bPrivate   = 0;
//
//    if ((opt = optget(opts, "PrivateMirror"))->enabled) {
//        /* Config specifies at least one PrivateMirror.
//         * Ignore the DatabaseMirrors. */
//        *bPrivate = 1;
//
//        do {
//            char *serverUrl = NULL;
//
//            if (cli_strbcasestr(opt->strarg, ".clamav.net")) {
//                logg("!The PrivateMirror config option may not include servers under *.clamav.net.\n");
//                status = FC_ECONFIG;
//                goto done;
//            }
//
//            if (FC_SUCCESS != (ret = get_server_node(opt->strarg, "http", &serverUrl))) {
//                mprintf("!get_database_server_list: Failed to read PrivateMirror server %s", opt->strarg);
//                status = ret;
//                goto done;
//            }
//
//            if (FC_SUCCESS != (ret = string_list_add(serverUrl, &servers, &numServers))) {
//                free(serverUrl);
//
//                mprintf("!get_database_server_list: Failed to add string to list.\n");
//                status = ret;
//                goto done;
//            }
//            free(serverUrl);
//        } while (NULL != (opt = opt->nextarg));
//    } else {
//        /* Check for DatabaseMirrors. */
//        if (!(opt = optget(opts, "DatabaseMirror"))->enabled) {
//            /* No DatabaseMirror configured. Fail out. */
//            logg("!No DatabaseMirror or PrivateMirror servers set in freshclam config file.\n");
//            status = FC_ECONFIG;
//            goto done;
//        }
//
//        do {
//            char *serverUrl = NULL;
//
//            if (FC_SUCCESS != (ret = get_server_node(opt->strarg, "https", &serverUrl))) {
//                mprintf("!get_database_server_list: Failed to parse DatabaseMirror server %s.", opt->strarg);
//                status = ret;
//                goto done;
//            }
//
//            if (FC_SUCCESS != (ret = string_list_add(serverUrl, &servers, &numServers))) {
//                free(serverUrl);
//
//                mprintf("!get_database_server_list: Failed to add string to list.\n");
//                status = ret;
//                goto done;
//            }
//            free(serverUrl);
//        } while (NULL != (opt = opt->nextarg));
//    }
//
//    *serverList = servers;
//    *nServers   = numServers;
//    status      = FC_SUCCESS;
//
//done:
//
//    if (FC_SUCCESS != status) {
//        free_string_list(servers, numServers);
//    }
//
//    return status;
//}
//
//static fc_error_t get_string_list(const struct optstruct *opt, char ***stringList, uint32_t *nListItems)
//{
//    fc_error_t ret;
//    fc_error_t status = FC_EARG;
//
//    char **newList  = NULL;
//    uint32_t nItems = 0;
//
//    if ((NULL == opt) || (NULL == stringList) || (NULL == nListItems)) {
//        mprintf("!get_string_list: Invalid arguments.\n");
//        goto done;
//    }
//
//    *stringList = NULL;
//    *nListItems = 0;
//
//    /* handle extra dbs */
//    if (opt->enabled) {
//        while (opt) {
//            if (FC_SUCCESS != (ret = string_list_add(opt->strarg, stringList, nListItems))) {
//                mprintf("!get_string_list: Failed to add string to list.\n");
//                status = ret;
//                goto done;
//            }
//            opt = opt->nextarg;
//        }
//    }
//
//    status = FC_SUCCESS;
//
//done:
//
//    if (FC_SUCCESS != status) {
//        free_string_list(newList, nItems);
//    }
//
//    return status;
//}
//
//static fc_error_t initialize(struct optstruct *opts)
//{
//    fc_error_t ret;
//    fc_error_t status = FC_EARG;
//    cl_error_t cl_init_retcode;
//    fc_config fcConfig;
//    char *tempDirectory                = NULL;
//    const struct optstruct *logFileOpt = NULL;
//
//    STATBUF statbuf;
//
//    memset(&fcConfig, 0, sizeof(fc_config));
//
//    if (NULL == opts) {
//        mprintf("!initialize: Invalid arguments.\n");
//        goto done;
//    }
//
//    /* Now that the config has been parsed,
//       check Foreground again if not already determined. */
//    if (g_foreground == -1) {
//        if (optget(opts, "Foreground")->enabled) {
//            g_foreground = 1;
//        } else {
//            g_foreground = 0;
//        }
//    }
//
//    /*
//     * Verify that the database directory exists.
//     * Create database directory if missing.
//     */
//    fcConfig.databaseDirectory = optget(opts, "DatabaseDirectory")->strarg;
//
//    if (LSTAT(fcConfig.databaseDirectory, &statbuf) == -1) {
//#ifdef HAVE_PWD_H
//        struct passwd *user;
//#endif
//
//        logg("Creating missing database directory: %s\n", fcConfig.databaseDirectory);
//
//        if (0 != mkdir(fcConfig.databaseDirectory, 0755)) {
//            logg("!Failed to create database directory: %s\n", fcConfig.databaseDirectory);
//            logg("Manually prepare the database directory, or re-run freshclam with higher privileges.\n");
//            status = FC_EDBDIRACCESS;
//            goto done;
//        }
//
//#ifdef HAVE_PWD_H
//        if (!geteuid()) {
//            /* Running as root user, will assign ownership of database directory to DatabaseOwner */
//            errno = 0;
//            if ((user = getpwnam(optget(opts, "DatabaseOwner")->strarg)) == NULL) {
//                logg("ERROR: Failed to get information about user \"%s\".\n",
//                     optget(opts, "DatabaseOwner")->strarg);
//                if (errno == 0) {
//                    logg("Create the \"%s\" user account for freshclam to use, or set the DatabaseOwner config option in freshclam.conf to a different user.\n",
//                         optget(opts, "DatabaseOwner")->strarg);
//                    logg("For more information, see https://docs.clamav.net/manual/Installing/Installing-from-source-Unix.html\n");
//                } else {
//                    logg("An unexpected error occurred when attempting to query the \"%s\" user account.\n",
//                         optget(opts, "DatabaseOwner")->strarg);
//                }
//                status = FC_EDBDIRACCESS;
//                goto done;
//            }
//
//            if (chown(fcConfig.databaseDirectory, user->pw_uid, user->pw_gid)) {
//                logg("!Failed to change database directory ownership to user %s. Error: %s\n", optget(opts, "DatabaseOwner")->strarg, strerror(errno));
//                status = FC_EDBDIRACCESS;
//                goto done;
//            }
//
//            logg("Assigned ownership of database directory to user \"%s\".\n", optget(opts, "DatabaseOwner")->strarg);
//        }
//#endif
//    }
//
//#ifdef HAVE_PWD_H
//    /* Drop database privileges here if we are not planning on daemonizing.  If
//     * we are, we should wait until after we craete the PidFile to drop
//     * privileges.  That way, it is owned by root (or whoever started freshclam),
//     * and no one can change it.  */
//    if (!optget(opts, "daemon")->enabled) {
//        /*
//         * freshclam shouldn't work with root privileges.
//         * Drop privileges to the DatabaseOwner user, if specified.
//         * Pass NULL for the log file name, because it hasn't been created yet.
//         */
//        ret = drop_privileges(optget(opts, "DatabaseOwner")->strarg, NULL);
//        if (ret) {
//            logg("!Failed to switch to %s user.\n", optget(opts, "DatabaseOwner")->strarg);
//            status = FC_ECONFIG;
//            goto done;
//        }
//    }
//#endif /* HAVE_PWD_H */
//
//    /*
//     * Initilize libclamav.
//     */
//    if (CL_SUCCESS != (cl_init_retcode = cl_init(CL_INIT_DEFAULT))) {
//        mprintf("!initialize: Can't initialize libclamav: %s\n", cl_strerror(cl_init_retcode));
//        status = FC_EINIT;
//        goto done;
//    }
//
//    /*
//     * Identify libfreshclam config options.
//     */
//    /* Set libclamav Message and [file-based] Logging option flags.
//       mprintf and logg options are also directly set, as they are also
//       used in freshclam (not only used in libfreshclam) */
//    if (optget(opts, "Debug")->enabled || optget(opts, "debug")->enabled)
//        fcConfig.msgFlags |= FC_CONFIG_MSG_DEBUG;
//
//    if ((optget(opts, "verbose")->enabled) ||
//        (optget(opts, "LogVerbose")->enabled)) {
//        fcConfig.msgFlags |= FC_CONFIG_MSG_VERBOSE;
//        fcConfig.logFlags |= FC_CONFIG_LOG_VERBOSE;
//    }
//
//    if (optget(opts, "quiet")->enabled) {
//        fcConfig.msgFlags |= FC_CONFIG_MSG_QUIET;
//        /* Silence libclamav messages. */
//        cl_set_clcb_msg(libclamav_msg_callback_quiet);
//    } else {
//        /* Enable libclamav messages, with [LibClamAV] message prefix. */
//        cl_set_clcb_msg(libclamav_msg_callback);
//    }
//
//    if (optget(opts, "no-warnings")->enabled) {
//        fcConfig.msgFlags |= FC_CONFIG_MSG_NOWARN;
//        fcConfig.logFlags |= FC_CONFIG_LOG_NOWARN;
//    }
//
//    if (optget(opts, "stdout")->enabled) {
//        fcConfig.msgFlags |= FC_CONFIG_MSG_STDOUT;
//    }
//
//    if (optget(opts, "show-progress")->enabled) {
//        fcConfig.msgFlags |= FC_CONFIG_MSG_SHOWPROGRESS;
//    }
//
//    if (optget(opts, "LogTime")->enabled) {
//        fcConfig.logFlags |= FC_CONFIG_LOG_TIME;
//    }
//    if (optget(opts, "LogFileMaxSize")->numarg && optget(opts, "LogRotate")->enabled) {
//        fcConfig.logFlags |= FC_CONFIG_LOG_ROTATE;
//    }
//    if (optget(opts, "LogSyslog")->enabled)
//        fcConfig.logFlags |= FC_CONFIG_LOG_SYSLOG;
//
//    logFileOpt = optget(opts, "UpdateLogFile");
//    if (logFileOpt->enabled) {
//        fcConfig.logFile = logFileOpt->strarg;
//    }
//    if (optget(opts, "LogFileMaxSize")->numarg) {
//        fcConfig.maxLogSize = optget(opts, "LogFileMaxSize")->numarg;
//    }
//
//#if defined(USE_SYSLOG) && !defined(C_AIX)
//    if (optget(opts, "LogSyslog")->enabled) {
//        if (optget(opts, "LogFacility")->enabled) {
//            fcConfig.logFacility = optget(opts, "LogFacility")->strarg;
//        }
//    }
//#endif
//
//    if ((optget(opts, "LocalIPAddress"))->enabled)
//        fcConfig.localIP = (optget(opts, "LocalIPAddress"))->strarg;
//
//    /* Select a path for the temp directory:  databaseDirectory/tmp */
//    tempDirectory          = cli_gentemp_with_prefix(fcConfig.databaseDirectory, "tmp");
//    fcConfig.tempDirectory = tempDirectory;
//
//    /* Store the path of the temp directory so we can delete it later. */
//    strncpy(g_freshclamTempDirectory, fcConfig.tempDirectory, sizeof(g_freshclamTempDirectory));
//    g_freshclamTempDirectory[sizeof(g_freshclamTempDirectory) - 1] = '\0';
//
//#ifndef _WIN32
//    /*
//     * If clamd.conf includes a HTTPProxyPassword,...
//     * ...make sure that permissions on the clamd.conf file aren't just wide open.
//     * If they are, fail out and warn the user so they will fix it.
//     */
//    if (optget(opts, "HTTPProxyPassword")->enabled) {
//        STATBUF statbuf;
//        const char *cfgfile = NULL;
//
//        cfgfile = optget(opts, "config-file")->strarg;
//        if (CLAMSTAT(cfgfile, &statbuf) == -1) {
//            logg("^Can't stat %s (critical error)\n", cfgfile);
//            status = FC_ECONFIG;
//            goto done;
//        }
//        if (statbuf.st_mode & (S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)) {
//            logg("^Insecure permissions (for HTTPProxyPassword): %s must have no more than 0700 permissions.\n", cfgfile);
//            status = FC_ECONFIG;
//            goto done;
//        }
//    }
//#endif
//
//    /* Initialize proxy settings */
//    if (optget(opts, "HTTPProxyServer")->enabled) {
//        fcConfig.proxyServer = optget(opts, "HTTPProxyServer")->strarg;
//        if (strncasecmp(fcConfig.proxyServer, "http://", strlen("http://")) == 0)
//            fcConfig.proxyServer += strlen("http://");
//
//        if (optget(opts, "HTTPProxyUsername")->enabled) {
//            fcConfig.proxyUsername = optget(opts, "HTTPProxyUsername")->strarg;
//            if (optget(opts, "HTTPProxyPassword")->enabled) {
//                fcConfig.proxyPassword = optget(opts, "HTTPProxyPassword")->strarg;
//            } else {
//                logg("HTTPProxyUsername requires HTTPProxyPassword\n");
//                status = FC_ECONFIG;
//                goto done;
//            }
//        }
//        if (optget(opts, "HTTPProxyPort")->enabled)
//            fcConfig.proxyPort = (uint16_t)optget(opts, "HTTPProxyPort")->numarg;
//        logg("Connecting via %s\n", fcConfig.proxyServer);
//    }
//
//    if (optget(opts, "HTTPUserAgent")->enabled) {
//
//        if (!(optget(opts, "PrivateMirror")->enabled) &&
//            (optget(opts, "DatabaseMirror")->enabled) &&
//            (strstr(optget(opts, "DatabaseMirror")->strarg, "clamav.net"))) {
//            /*
//             * Using the official project CDN.
//             */
//            logg("In an effort to reduce CDN data costs, HTTPUserAgent may not be used when updating from clamav.net.\n");
//            logg("The HTTPUserAgent specified in your config will be ignored so that FreshClam is not blocked by the CDN.\n");
//            logg("If ClamAV's user agent is not allowed through your firewall/proxy, please contact your network administrator.\n\n");
//        } else {
//            /*
//             * Using some other CDN or private mirror.
//             */
//            fcConfig.userAgent = optget(opts, "HTTPUserAgent")->strarg;
//        }
//    }
//
//    fcConfig.maxAttempts    = optget(opts, "MaxAttempts")->numarg;
//    fcConfig.connectTimeout = optget(opts, "ConnectTimeout")->numarg;
//    fcConfig.requestTimeout = optget(opts, "ReceiveTimeout")->numarg;
//
//    fcConfig.bCompressLocalDatabase = optget(opts, "CompressLocalDatabase")->enabled;
//
//    /*
//     * Initilize libfreshclam.
//     */
//    if (FC_SUCCESS != (ret = fc_initialize(&fcConfig))) {
//        mprintf("!initialize: libfreshclam init failed.\n");
//        status = ret;
//        goto done;
//    }
//
//    /*
//     * Set libfreshclam callback functions.
//     */
//    fc_set_fccb_download_complete(download_complete_callback);  
//
//    status = FC_SUCCESS;
//
//done:
//    if (NULL != tempDirectory) {
//        free(tempDirectory);
//    }
//
//    return status;
//}
//
//
//fc_error_t get_official_database_lists(
//    char ***standardDatabases,
//    uint32_t *nStandardDatabases,
//    char ***optionalDatabases,
//    uint32_t *nOptionalDatabases)
//{
//    fc_error_t ret;
//    fc_error_t status = FC_EARG;
//    uint32_t i;
//
//    const char *hardcodedStandardDatabaseList[] = {"daily", "main", "bytecode"};
//    const char *hardcodedOptionalDatabaseList[] = {"safebrowsing", "test"};
//
//    if ((NULL == standardDatabases) || (NULL == nStandardDatabases) || (NULL == optionalDatabases) || (NULL == nOptionalDatabases)) {
//        mprintf("!get_official_database_lists: Invalid arguments.\n");
//        goto done;
//    }
//
//    *standardDatabases  = NULL;
//    *nStandardDatabases = 0;
//    *optionalDatabases  = NULL;
//    *nOptionalDatabases = 0;
//
//    for (i = 0; i < sizeof(hardcodedStandardDatabaseList) / sizeof(hardcodedStandardDatabaseList[0]); i++) {
//        if (FC_SUCCESS != (ret = string_list_add(hardcodedStandardDatabaseList[i], standardDatabases, nStandardDatabases))) {
//            logg("!Failed to add %s to list of standard databases.\n", hardcodedStandardDatabaseList[i]);
//            status = ret;
//            goto done;
//        }
//    }
//
//    for (i = 0; i < sizeof(hardcodedOptionalDatabaseList) / sizeof(hardcodedOptionalDatabaseList[0]); i++) {
//        if (FC_SUCCESS != (ret = string_list_add(hardcodedOptionalDatabaseList[i], optionalDatabases, nOptionalDatabases))) {
//            logg("!Failed to add %s to list of optional databases.\n", hardcodedOptionalDatabaseList[i]);
//            status = ret;
//            goto done;
//        }
//    }
//
//    logg("*Collected lists of official standard and optional databases.\n");
//
//    status = FC_SUCCESS;
//
//done:
//
//    if (FC_SUCCESS != status) {
//        if ((NULL != standardDatabases) && (*standardDatabases != NULL) && (nStandardDatabases != NULL)) {
//            free_string_list(*standardDatabases, *nStandardDatabases);
//            *standardDatabases  = NULL;
//            *nStandardDatabases = 0;
//        }
//        if ((NULL != optionalDatabases) && (*optionalDatabases != NULL) && (nOptionalDatabases != NULL)) {
//            free_string_list(*optionalDatabases, *nOptionalDatabases);
//            *optionalDatabases  = NULL;
//            *nOptionalDatabases = 0;
//        }
//    }
//
//    return status;
//}
//
//fc_error_t select_from_official_databases(
//    char **optInList,
//    uint32_t nOptIns,
//    char **optOutList,
//    uint32_t nOptOuts,
//    char ***databaseList,
//    uint32_t *nDatabases)
//{
//    fc_error_t ret;
//    fc_error_t status = FC_EARG;
//
//    char **standardDatabases    = NULL;
//    uint32_t nStandardDatabases = 0;
//    char **optionalDatabases    = NULL;
//    uint32_t nOptionalDatabases = 0;
//    char **selectedDatabases    = NULL;
//    uint32_t nSelectedDatabases = 0;
//    uint32_t i;
//
//    if ((NULL == databaseList) || (0 == nDatabases)) {
//        mprintf("!select_from_official_databases: Invalid arguments.\n");
//        goto done;
//    }
//
//    *databaseList = NULL;
//    *nDatabases   = 0;
//
//    if ((0 < nOptIns) && (NULL == optInList)) {
//        mprintf("!select_from_official_databases: Invalid arguments. Number of opt-in databases does not match empty database array.\n");
//        goto done;
//    }
//
//    if ((0 < nOptOuts) && (NULL == optOutList)) {
//        mprintf("!select_from_official_databases: Invalid arguments. Number of opt-out databases does not match empty database array.\n");
//        goto done;
//    }
//
//    /*
//     * Get lists of available databases.
//     */
//    if (FC_SUCCESS != (ret = get_official_database_lists(&standardDatabases, &nStandardDatabases, &optionalDatabases, &nOptionalDatabases))) {
//        logg("!Failed to get lists of official standard and optional databases.\n");
//        status = ret;
//        goto done;
//    }
//
//    selectedDatabases = cli_calloc(nStandardDatabases + nOptionalDatabases, sizeof(char *));
//
//    /*
//     * Select desired standard databases.
//     */
//    for (i = 0; i < nStandardDatabases; i++) {
//        uint32_t j;
//        int skip = 0;
//
//        for (j = 0; j < nOptOuts; j++) {
//            if (0 == strcasecmp(standardDatabases[i], optOutList[j])) {
//                skip = 1;
//            }
//        }
//
//        if (skip) {
//            logg("*Opting out of standard database: %s\n", standardDatabases[i]);
//            continue;
//        }
//
//        logg("*Selecting standard database: %s\n", standardDatabases[i]);
//        if (FC_SUCCESS != (ret = string_list_add(standardDatabases[i], &selectedDatabases, &nSelectedDatabases))) {
//            logg("!Failed to add standard database %s to list of selected databases.\n", standardDatabases[i]);
//            status = ret;
//            goto done;
//        }
//    }
//
//    /*
//     * Select desired optional databases.
//     */
//    for (i = 0; i < nOptIns; i++) {
//        uint32_t j;
//        int found = 0;
//
//        for (j = 0; j < nOptionalDatabases; j++) {
//            if (0 == strcasecmp(optInList[i], optionalDatabases[j])) {
//                found = 1;
//            }
//        }
//
//        if (!found) {
//            logg("^Desired optional database \"%s\" is not available.\n", optInList[i]);
//            continue;
//        }
//
//        logg("*Selecting optional database: %s\n", optInList[i]);
//        if (FC_SUCCESS != (ret = string_list_add(optInList[i], &selectedDatabases, &nSelectedDatabases))) {
//            logg("!Failed to add optional database %s to list of selected databases.\n", optInList[i]);
//            status = ret;
//            goto done;
//        }
//    }
//
//    *databaseList = selectedDatabases;
//    *nDatabases   = nSelectedDatabases;
//
//    status = FC_SUCCESS;
//
//done:
//
//    if (NULL != standardDatabases) {
//        free_string_list(standardDatabases, nStandardDatabases);
//    }
//    if (NULL != optionalDatabases) {
//        free_string_list(optionalDatabases, nOptionalDatabases);
//    }
//    if (FC_SUCCESS != status) {
//        if (NULL != selectedDatabases) {
//            free_string_list(selectedDatabases, nSelectedDatabases);
//        }
//    }
//
//    return status;
//}
//
//static fc_error_t executeIfNewVersion(
//    const char *command,
//    char *newVersion,
//    int bDaemonized)
//{
//    fc_error_t status = FC_EARG;
//
//    char *modifiedCommand = NULL;
//    char *replace_version = NULL;
//
//    if ((NULL == command) || (NULL == newVersion)) {
//        logg("!executeIfNewVersion: Invalid args\n");
//        status = FC_EARG;
//        goto done;
//    }
//
//    if (NULL == (replace_version = strstr(command, "%v"))) {
//        /*
//         * Execute command as-is.
//         */
//        execute("OnOutdatedExecute", command, bDaemonized);
//    } else {
//        /*
//         * Replace "%v" with version numbers, then execute command.
//         */
//        char *after_replace_version = NULL;
//        char *version               = newVersion;
//
//        while (*version) {
//            if (!strchr("0123456789.", *version)) {
//                logg("!executeIfNewVersion: OnOutdatedExecute: Incorrect version number string\n");
//                status = FC_EARG;
//                goto done;
//            }
//            version++;
//        }
//        modifiedCommand = (char *)malloc(strlen(command) + strlen(version) + 10);
//        if (NULL == modifiedCommand) {
//            logg("!executeIfNewVersion: Can't allocate memory for modifiedCommand\n");
//            status = FC_EMEM;
//            goto done;
//        }
//
//        /* Copy first half of command */
//        strncpy(modifiedCommand, command, replace_version - command);
//        modifiedCommand[replace_version - command] = '\0'; /* Add null terminator */
//
//        /* Cat on the version number */
//        strcat(modifiedCommand, version);
//
//        /* Cat on the rest of the command */
//        after_replace_version = replace_version + 2;
//        strcat(modifiedCommand, after_replace_version);
//
//        /* Make it so. */
//        execute("OnOutdatedExecute", modifiedCommand, bDaemonized);
//    }
//
//    status = FC_SUCCESS;
//
//done:
//
//    if (NULL != modifiedCommand) {
//        free(modifiedCommand);
//    }
//
//    return status;
//}
//
//fc_error_t perform_database_update(
//    char **databaseList,
//    uint32_t nDatabases,
//    char **urlDatabaseList,
//    uint32_t nUrlDatabases,
//    char **serverList,
//    uint32_t nServers,
//    int bPrivateMirror,
//    const char *dnsUpdateInfoServer,
//    int bScriptedUpdates,
//    int bPrune,
//    const char *onUpdateExecute,
//    const char *onOutdatedExecute,
//    int bDaemonized,
//    char *notifyClamd,
//    fc_ctx *fc_context)
//{
//    fc_error_t ret;
//    fc_error_t status = FC_EARG;
//    time_t currtime;
//    char *dnsUpdateInfo    = NULL;
//    char *newVersion       = NULL;
//    uint32_t nUpdated      = 0;
//    uint32_t nTotalUpdated = 0;
//
//    STATBUF statbuf;
//
//    if (NULL == serverList) {
//        mprintf("!perform_database_update: Invalid arguments.\n");
//        goto done;
//    }
//    if (((NULL == databaseList) || (0 == nDatabases)) &&
//        ((NULL == urlDatabaseList) || (0 == nUrlDatabases))) {
//        mprintf("!perform_database_update: No databases requested.\n");
//        goto done;
//    }
//
//    time(&currtime);
//    logg("ClamAV update process started at %s", ctime(&currtime));
//
//    if (bPrune) {
//        /*
//         * Prune database directory of official databases
//         * that are no longer available or no longer desired.
//         */
//        (void)fc_prune_database_directory(databaseList, nDatabases);
//    }
//
//    /*
//     * Query DNS (if enabled) to get Update Info.
//     */
//    (void)fc_dns_query_update_info(dnsUpdateInfoServer, &dnsUpdateInfo, &newVersion);
//
//    /*
//     * Create a temp directory to use for the update process.
//     */
//    if (LSTAT(g_freshclamTempDirectory, &statbuf) == -1) {
//        if (0 != mkdir(g_freshclamTempDirectory, 0700)) {
//            logg("!Can't create temporary directory %s\n", g_freshclamTempDirectory);
//            logg("Hint: The database directory must be writable for UID %d or GID %d\n", getuid(), getgid());
//            status = FC_EDBDIRACCESS;
//            goto done;
//        }
//    }
//
//    if ((NULL != databaseList) && (0 < nDatabases)) {
//        /*
//        * Download/update the desired official databases.
//         */
//        ret = fc_update_databases(
//            databaseList,
//            nDatabases,
//            serverList,
//            nServers,
//            bPrivateMirror,
//            dnsUpdateInfo,
//            bScriptedUpdates,
//            (void *)fc_context,
//            &nUpdated);
//        if (FC_SUCCESS != ret) {
//            logg("!Database update process failed: %s\n", fc_strerror(ret));
//            status = ret;
//            goto done;
//        }
//        nTotalUpdated += nUpdated;
//    }
//
//    if ((NULL != urlDatabaseList) && (0 < nUrlDatabases)) {
//        /*
//         * Download/update the desired unofficial / URL-based databases.
//         */
//        ret = fc_download_url_databases(
//            urlDatabaseList,
//            nUrlDatabases,
//            (void *)fc_context,
//            &nUpdated);
//        if (FC_SUCCESS != ret) {
//            logg("!Database update process failed: %s\n", fc_strerror(ret));
//            status = ret;
//            goto done;
//        }
//        nTotalUpdated += nUpdated;
//
//        logg("*Database update completed successfully.\n");
//    }
//
//    if (0 < nTotalUpdated) {
//        if (NULL != notifyClamd) {
//            notify(notifyClamd);
//        }
//    }
//
//    status = FC_SUCCESS;
//
//done:
//
//    if (LSTAT(g_freshclamTempDirectory, &statbuf) != -1) {
//        /* Remove temp directory */
//        if (*g_freshclamTempDirectory) {
//            cli_rmdirs(g_freshclamTempDirectory);
//        }
//    }
//
//    if (FC_SUCCESS == status) {
//        /* Run Execute commands after we clean up the temp directory,
//         * in case they want us to EXIT */
//        if (0 < nTotalUpdated) {
//            if (NULL != onUpdateExecute) {
//                execute("OnUpdateExecute", onUpdateExecute, bDaemonized);
//            }
//        }
//        if ((NULL != newVersion) && (NULL != onOutdatedExecute)) {
//            executeIfNewVersion(onOutdatedExecute, newVersion, bDaemonized);
//        }
//    }
//
//    if (NULL != dnsUpdateInfo) {
//        free(dnsUpdateInfo);
//    }
//    if (NULL != newVersion) {
//        free(newVersion);
//    }
//
//    return status;
//}

int main(int argc, char **argv)
{
    printf("Hello World\n");
    /*fc_error_t ret;
    fc_error_t status = FC_ECONNECTION;
    char *cfgfile     = NULL;
    const char *arg   = NULL;
    struct optstruct *opts = NULL;
    const struct optstruct *opt;

    char **serverList               = NULL;
    uint32_t nServers               = 0;
    int bPrivate                    = 0;
    const char *dnsUpdateInfoServer = NULL; 

    char **databaseList    = NULL;
    uint32_t nDatabases    = 0;
    char **urlDatabaseList = NULL;
    uint32_t nUrlDatabases = 0;


    int bPrune = 1; 
    fc_ctx fc_context = {0}; 
    int i;*/

    int fd, har;
    unsigned long int size = 0;
    unsigned int sigs      = 0;
    long double mb;
    const char *virname;
    const char *filename;
    struct cl_engine *engine;
    struct cl_scan_options options;
    char up[] = "update_check"; 

    if (argc != 2) {
        printf("Usage: %s file\n", argv[0]);
        return 2;
    }

    filename = argv[1];

    /* Tip: Scan input should be opened as binary on Windows! */
    if ((fd = open(argv[1], O_RDONLY | O_BINARY)) == -1) {
        printf("Can't open file %s\n", argv[1]);
        return 2;
    }

    if ((har = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) { 
        printf("Can't initialize libclamav: %s\n", cl_strerror(har));
        return 2;
    }

    if (!(engine = cl_engine_new())) {
        printf("Can't create new engine\n");
        return 2;
    }

    /* Example version macro usage to determine if new feature is available */
#if defined(LIBCLAMAV_VERSION_NUM) && (LIBCLAMAV_VERSION_NUM >= 0x090400)
    /* Example feature usage lowering max scan time to 15 seconds. */
    cl_engine_set_num(engine, CL_ENGINE_MAX_SCANTIME, 15000);
#endif

    /* load all available databases from default directory */
    if ((har = cl_load(cl_retdbdir(), engine, &sigs, CL_DB_STDOPT)) != CL_SUCCESS) {
        printf("cl_load: %s\n", cl_strerror(har));
        close(fd);
        cl_engine_free(engine);
        return 2;
    }

    printf("Loaded %u signatures.\n", sigs);
    
    /* build engine */
    if ((har = cl_engine_compile(engine)) != CL_SUCCESS) {
        printf("Database initialization error: %s\n", cl_strerror(har));
        cl_engine_free(engine);
        close(fd);
        return 2;
    }

    /* scan file descriptor */
    memset(&options, 0, sizeof(struct cl_scan_options));
    options.parse |= ~0;                           /* enable all parsers */
    options.general |= CL_SCAN_GENERAL_HEURISTICS; /* enable heuristic alert options */

    if ((ret = cl_scandesc(fd, filename, &virname, &size, engine, &options)) == CL_VIRUS) {
        printf("Virus detected: %s\n", virname);
    } else {
        if (ret == CL_CLEAN) {
            printf("No virus detected.\n");
        } else {
            printf("Error: %s\n", cl_strerror(ret));
            cl_engine_free(engine);
            close(fd);
            return 2;
        }
    }
    /*int out = strcmp(filename, up);*/
    //if (out == 0) {
    //    printf("update function selected\n");

    //    if ((opts = optparse(NULL, argc, argv, 1, OPT_FRESHCLAM, 0, NULL)) == NULL) {
    //        mprintf("!Can't parse command line options\n");
    //        status = FC_EINIT;
    //        goto done;
    //    } 

    //    /*
    //     * Handle options that immediately exit.
    //     */
    //    if (optget(opts, "version")->enabled) {
    //        print_version(optget(opts, "DatabaseDirectory")->strarg);
    //        status = FC_SUCCESS;
    //        goto done;
    //    }
    //    if (optget(opts, "list-mirrors")->enabled) {
    //        mprintf("^Deprecated option --list-mirrors. Individual mirrors are no longer tracked, as official signature distribution is now done through the CloudFlare CDN.\n");
    //        status = FC_SUCCESS;
    //        goto done;
    //    }

    //    /*
    //     * Collect list of database servers from DatabaseMirror(s) or PrivateMirror(s).
    //     */
    //    if (FC_SUCCESS != (ret = get_database_server_list(opts, &serverList, &nServers, &bPrivate))) {
    //        mprintf("!Unable to find DatabaseMirror or PrivateMirror option(s) that specify database server FQDNs.\n");
    //        status = ret;
    //        goto done;
    //    }
    //    /*
    //     * Prep for standard database updates.
    //     */
    //    char **optInList  = NULL;
    //    uint32_t nOptIns  = 0;
    //    char **optOutList = NULL;
    //    uint32_t nOptOuts = 0;

    //    /*
    //     * Parse the config file.
    //     */
    //    cfgfile = cli_strdup(optget(opts, "config-file")->strarg);
    //    if ((opts = optparse(cfgfile, 0, NULL, 1, OPT_FRESHCLAM, 0, opts)) == NULL) {
    //        fprintf(stderr, "ERROR: Can't open/parse the config file %s\n", cfgfile);
    //        status = FC_EINIT;
    //        goto done;
    //    }

    //    /*
    //     * Collect list of database opt-ins.
    //     */
    //    if (FC_SUCCESS != (ret = get_string_list(optget(opts, "ExtraDatabase"), &optInList, &nOptIns))) {
    //        mprintf("!Error when attempting to read ExtraDatabase entries.\n");
    //        status = ret;
    //        goto done;
    //    } 

    //    /*
    //     * Collect list of database opt-outs.
    //     */
    //    if (FC_SUCCESS != (ret = get_string_list(optget(opts, "ExcludeDatabase"), &optOutList, &nOptOuts))) {
    //        free_string_list(optInList, nOptIns);
    //        optInList = NULL;

    //        mprintf("!Error when attempting to read ExcludeDatabase entries.\n");
    //        status = ret;
    //        goto done;
    //    }
    //    if (!optget(opts, "Bytecode")->enabled) {
    //        if (FC_SUCCESS != (ret = string_list_add("bytecode", &optOutList, &nOptOuts))) {
    //            free_string_list(optInList, nOptIns);
    //            optInList = NULL;
    //            free_string_list(optOutList, nOptOuts);
    //            optOutList = NULL;

    //            mprintf("!Failed to add bytecode to list of opt-out databases.\n");
    //            status = ret;
    //            goto done;
    //        }
    //    }

    //    /*
    //     * Select databases from official lists using opt-ins and opt-outs.
    //     */
    //    if (FC_SUCCESS != (ret = select_from_official_databases(
    //                           optInList,
    //                           nOptIns,
    //                           optOutList,
    //                           nOptOuts,
    //                           &databaseList,
    //                           &nDatabases))) {
    //        free_string_list(optInList, nOptIns);
    //        optInList = NULL;
    //        free_string_list(optOutList, nOptOuts);
    //        optOutList = NULL;

    //        mprintf("!Failed to select databases from list of official databases.\n");
    //        status = ret;
    //        goto done;
    //    }
    //    free_string_list(optInList, nOptIns);
    //    optInList = NULL;
    //    free_string_list(optOutList, nOptOuts);
    //    optOutList = NULL;

    //    /*
    //     * Collect list of "custom"/unofficial URL-based databases.
    //     */
    //    if (FC_SUCCESS != (ret = get_string_list(optget(opts, "DatabaseCustomURL"), &urlDatabaseList, &nUrlDatabases))) {
    //        mprintf("!Error when attempting to read ExcludeDatabase entries.\n");
    //        status = ret;
    //        goto done;
    //    }

    //    fc_context.bTestDatabases   = optget(opts, "TestDatabases")->enabled;
    //    fc_context.bBytecodeEnabled = optget(opts, "Bytecode")->enabled;

    //    /*
    //     * Initialize libraries and configuration options.
    //     */
    //    if (FC_SUCCESS != initialize(opts)) {
    //        mprintf("!Initialization error!\n");
    //        status = FC_EINIT;
    //        goto done;
    //    }

    //    if (!optget(opts, "no-dns")->enabled && optget(opts, "DNSDatabaseInfo")->enabled) {
    //        dnsUpdateInfoServer = optget(opts, "DNSDatabaseInfo")->strarg;
    //    }

    //    /*
    //     * Daemon mode not enabled.
    //     * Just update and exit.
    //     */
    //    ret = perform_database_update(
    //        databaseList,
    //        nDatabases,
    //        urlDatabaseList,
    //        nUrlDatabases,
    //        serverList,
    //        nServers,
    //        bPrivate,
    //        bPrivate ? NULL : dnsUpdateInfoServer,
    //        bPrivate ? 0 : optget(opts, "ScriptedUpdates")->enabled,
    //        bPrune,
    //        optget(opts, "OnUpdateExecute")->enabled ? optget(opts, "OnUpdateExecute")->strarg : NULL,
    //        optget(opts, "OnOutdatedExecute")->enabled ? optget(opts, "OnOutdatedExecute")->strarg : NULL,
    //        optget(opts, "daemon")->enabled,
    //        optget(opts, "NotifyClamd")->active ? optget(opts, "NotifyClamd")->strarg : NULL,
    //        &fc_context);
    //     
    //    if (FC_SUCCESS != ret) {
    //        printf("Update failed\n");
    //        status = ret;
    //        goto done; 
    //    }

     /*else {*/
        if ((har = cl_scanfile(filename, &virname, NULL, engine, &options)) == CL_VIRUS) {
            printf("Virus detected: %s\n", virname);
        } else {
            if (har == CL_CLEAN) {
                printf("No virus detected.\n");
            } else {
                printf("Error: %s\n", cl_strerror(har));
            }
        }
    /*}*/
    
    close(fd);
  done:
    /* free memory */
    cl_engine_free(engine);

    /* calculate size of scanned data */
    mb = size * (CL_COUNT_PRECISION / 1024) / 1024.0;
    printf("Data scanned: %2.2Lf MB\n", mb);

    return har == CL_VIRUS ? 1 : 0;
}
