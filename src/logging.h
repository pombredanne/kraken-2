// logging.h
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following disclaimer
//   in the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of SecureState Consulting nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef _KRAKEN_LOGGING_H
#define _KRAKEN_LOGGING_H

/*
 * Acceptable Log Levels For Log4C:
 *     fatal:    LOG4C_PRIORITY_FATAL
 *     alert:    LOG4C_PRIORITY_ALERT
 *     critical: LOG4C_PRIORITY_CRIT
 *     error:    LOG4C_PRIORITY_ERROR
 *     warning:  LOG4C_PRIORITY_WARN
 *     notice:   LOG4C_PRIORITY_NOTICE
 *     info:     LOG4C_PRIORITY_INFO
 *     debug:    LOG4C_PRIORITY_DEBUG
 *     trace:    LOG4C_PRIORITY_TRACE
 *     notset:   LOG4C_PRIORITY_NOTSET
 *     unknown:  LOG4C_PRIORITY_UNKNOWN
 */

#ifndef WITHOUT_LOG4C

#include <log4c.h>
#define LOGGING_FATAL LOG4C_PRIORITY_FATAL
#define LOGGING_ALERT LOG4C_PRIORITY_ALERT
#define LOGGING_CRITICAL LOG4C_PRIORITY_CRIT
#define LOGGING_ERROR LOG4C_PRIORITY_ERROR
#define LOGGING_WARNING LOG4C_PRIORITY_WARN
#define LOGGING_NOTICE LOG4C_PRIORITY_NOTICE
#define LOGGING_INFO LOG4C_PRIORITY_INFO
#define LOGGING_DEBUG LOG4C_PRIORITY_DEBUG
#define LOGGING_TRACE LOG4C_PRIORITY_TRACE
#define LOGGING_NOTSET LOG4C_PRIORITY_NOTSET
#define LOGGING_UNKNOWN LOG4C_PRIORITY_UNKNOWN

#define LOGGING_QUICK(catName, priority, msg) log4c_category_log(log4c_category_get(catName), priority, msg);
#define LOGGING_QUICK_FATAL(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_FATAL, msg);
#define LOGGING_QUICK_ALERT(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_ALERT, msg);
#define LOGGING_QUICK_CRITICAL(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_CRIT, msg);
#define LOGGING_QUICK_ERROR(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_ERROR, msg);
#define LOGGING_QUICK_WARNING(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_WARN, msg);
#define LOGGING_QUICK_NOTICE(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_NOTICE, msg);
#define LOGGING_QUICK_INFO(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_INFO, msg);
#define LOGGING_QUICK_DEBUG(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_DEBUG, msg);
#define LOGGING_QUICK_TRACE(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_TRACE, msg);
#define LOGGING_QUICK_NOTSET(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_NOTSET, msg);
#define LOGGING_QUICK_UNKNOWN(catName, msg) log4c_category_log(log4c_category_get(catName), LOG4C_PRIORITY_UNKNOWN, msg);

#else

#define LOGGING_FATAL 0
#define LOGGING_ALERT 0
#define LOGGING_CRITICAL 0
#define LOGGING_ERROR 0
#define LOGGING_WARNING 0
#define LOGGING_NOTICE 0
#define LOGGING_INFO 0
#define LOGGING_DEBUG 0
#define LOGGING_TRACE 0
#define LOGGING_NOTSET 0
#define LOGGING_UNKNOWN 0

#define LOGGING_QUICK(catName, priority, msg) ((void) 0);
#define LOGGING_QUICK_FATAL(catName, msg) ((void) 0);
#define LOGGING_QUICK_ALERT(catName, msg) ((void) 0);
#define LOGGING_QUICK_CRITICAL(catName, msg) ((void) 0);
#define LOGGING_QUICK_ERROR(catName, msg) ((void) 0);
#define LOGGING_QUICK_WARNING(catName, msg) ((void) 0);
#define LOGGING_QUICK_NOTICE(catName, msg) ((void) 0);
#define LOGGING_QUICK_INFO(catName, msg) ((void) 0);
#define LOGGING_QUICK_DEBUG(catName, msg) ((void) 0);
#define LOGGING_QUICK_TRACE(catName, msg) ((void) 0);
#define LOGGING_QUICK_NOTSET(catName, msg) ((void) 0);
#define LOGGING_QUICK_UNKNOWN(catName, msg) ((void) 0);

#endif

void logging_log(const char *catName, const int priority, const char *format, ...);

#endif
