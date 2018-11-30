/*
 *  sput - Simple, Portable Unit Testing Framework for C/C++ v1.4.0
 *  Version 1.4.0 erweitert um JUnit-ähnliche-XML-Ausgabe
 *
 *              http://www.use-strict.de/sput-unit-testing/
 *              https://github.com/408b7f8b/sput_report
 *
 *
 *  Copyright (C) 2011-2015 Lingua-Systems Software GmbH
 *  Copyright (C) 2016 Alex Linke <alex@use-strict.de>
 *  Copyright (C) 2018 D. Breunig
 *
 *  All rights reserved.
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *  TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef HAVE_SPUT_H
#define HAVE_SPUT_H

#define MY_ENCODING "UTF-8"

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

void getDateTime(char* buffer) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    struct tm* tm_info;
    tm_info = localtime(&tv.tv_sec);

    strftime(buffer, 32, "%Y-%m-%dT%H:%M:%S %Z", tm_info);
}

    /* ===================================================================
     *                             definitions
     * =================================================================== */

#define SPUT_VERSION_MAJOR      1
#define SPUT_VERSION_MINOR      4
#define SPUT_VERSION_PATCH      0
#define SPUT_VERSION_STRING     "1.4.0"

#define SPUT_DEFAULT_SUITE_NAME "Unlabeled Suite"
#define SPUT_DEFAULT_CHECK_NAME "Unlabeled Check"

#define SPUT_INITIALIZED        0x06 /* ACK */


    /* ===================================================================
     *                        sput global variable
     * =================================================================== */

    struct sput_time
    {
        time_t start;
        time_t end;
    };

    struct sput_check
    {
        const char   *name;
        const char   *cond;
        const char   *type;
        unsigned long line;

        char failure;
    };

    struct sput_test
    {
        const char   *name;
        const char* classname;
        unsigned long nr;

        struct sput_time time;

        struct sput_check* checksZgr;
        unsigned long checks;
        char failure;
    };

    struct sput_suite
    {
        const char   *name;
        unsigned long nr;
        unsigned long checks;
        unsigned long ok;
        unsigned long nok;

        struct sput_time time;
        char timestamp[32];

        struct sput_test* testsZgr;
        unsigned long tests;
        unsigned long test_failures;
    };

    struct sput_overall
    {
        unsigned long checks;
        unsigned long suites;
        unsigned long ok;
        unsigned long nok;

        struct sput_time time;

        struct sput_suite* suitesZgr;
    };

    static struct sput
    {
        FILE *out;
        char  initialized;

        struct sput_check check;
        struct sput_test test;
        struct sput_suite suite;
        struct sput_overall overall;

        char* filename;
    } __sput;


    /* ==================================================================
     *                        sput internal macros
     * ================================================================== */

#define _sput_die_unless_initialized()                                     \
    if (__sput.initialized != SPUT_INITIALIZED)                            \
    {                                                                      \
        fputs("sput_start_testing() omitted\n", stderr);                   \
        exit(EXIT_FAILURE);                                                \
    }


#define _sput_die_unless_suite_set()                                       \
    if (! __sput.suite.name)                                               \
    {                                                                      \
        fputs("sput_enter_suite() omitted\n", __sput.out);                 \
        exit(EXIT_FAILURE);                                                \
    }


#define _sput_die_unless_test_set()                                        \
    if (! __sput.test.name)                                                \
    {                                                                      \
        fputs("sput_run_test() omitted\n", __sput.out);                    \
        exit(EXIT_FAILURE);                                                \
    }


#define _sput_check_failed()                                               \
    {                                                                      \
        _sput_die_unless_initialized();                                    \
        _sput_die_unless_suite_set();                                      \
        __sput.suite.nok++;                                                \
        __sput.test.failure = 1;                                           \
        __sput.check.failure = 1;                                          \
        fprintf(__sput.out,                                                \
                "[%lu:%lu]  %s:#%lu  \"%s\"  FAIL\n"                       \
                "!    Type:      %s\n"                                     \
                "!    Condition: %s\n"                                     \
                "!    Line:      %lu\n",                                   \
                __sput.suite.nr, __sput.suite.checks, __sput.test.name,    \
                __sput.test.nr, __sput.check.name, __sput.check.type,      \
                __sput.check.cond, __sput.check.line);                     \
    }


#define _sput_check_succeeded()                                            \
    {                                                                      \
        _sput_die_unless_initialized();                                    \
        _sput_die_unless_suite_set();                                      \
        __sput.suite.ok++;                                                 \
        __sput.check.failure = 0;                                          \
        fprintf(__sput.out,                                                \
                "[%lu:%lu]  %s:#%lu  \"%s\"  pass\n",                      \
                __sput.suite.nr, __sput.suite.checks,                      \
                __sput.test.name,                                          \
                __sput.test.nr,                                            \
                __sput.check.name);                                        \
    }


    /* ==================================================================
     *                            user macros
     * ================================================================== */


#define sput_check_sichern()\
    do {\
        if(__sput.test.checksZgr == NULL){\
            __sput.test.checksZgr = (struct sput_check*)malloc(sizeof(struct sput_check));\
        }else{\
            struct sput_check* n = realloc(__sput.test.checksZgr, __sput.test.checks*sizeof(struct sput_check));\
            if(n != NULL)\
                __sput.test.checksZgr = n;\
        }\
        if(__sput.test.checksZgr != NULL){\
            memcpy(&(__sput.test.checksZgr[__sput.test.checks-1]), &(__sput.check), sizeof(struct sput_check));\
        }\
    } while (0)

#define sput_test_sichern()\
    do {\
        if(__sput.test.name != NULL){\
            if(__sput.suite.testsZgr == NULL){\
                __sput.suite.testsZgr = (struct sput_test*)malloc(sizeof(struct sput_test));\
            }else{\
                struct sput_test* n = realloc(__sput.suite.testsZgr, __sput.suite.tests*sizeof(struct sput_test));\
                if(n != NULL)\
                    __sput.suite.testsZgr = n;\
            }\
            if(__sput.suite.testsZgr != NULL){\
                memcpy(&(__sput.suite.testsZgr[__sput.suite.tests-1]), &(__sput.test), sizeof(struct sput_test));\
                struct sput_test* t = &(__sput.suite.testsZgr[__sput.suite.tests-1]);\
            }\
        }\
    } while (0)

#define sput_suite_sichern()\
    do {\
        if(__sput.overall.suitesZgr == NULL){\
            __sput.overall.suitesZgr = (struct sput_suite*)malloc(sizeof(struct sput_suite));\
        }else{\
            struct sput_suite* n = realloc(__sput.overall.suitesZgr, __sput.overall.suites*sizeof(struct sput_suite));\
            if(n != NULL)\
                __sput.overall.suitesZgr = n;\
        }\
        if(__sput.overall.suitesZgr != NULL){\
            memcpy(&(__sput.overall.suitesZgr[__sput.overall.suites-1]), &(__sput.suite), sizeof(struct sput_suite));\
        }\
    } while (0)

void sput_write_xml_f(char* filename) {
    char buffer[255];
    int rc;
    xmlTextWriterPtr writer;
    writer = xmlNewTextWriterFilename(filename, 0);

    rc = xmlTextWriterSetIndent(writer, 1);

    rc = xmlTextWriterStartDocument(writer, NULL, MY_ENCODING, NULL);
    rc = xmlTextWriterStartElement(writer, BAD_CAST "testsuites");

    int i = 0;
    for (; i < __sput.overall.suites; ++i) {

        struct sput_suite* suite = &(__sput.overall.suitesZgr[i]);

        rc = xmlTextWriterStartElement(writer, BAD_CAST "testsuite");
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST suite->name);
        sprintf(buffer, "%li", suite->tests);
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "tests", BAD_CAST buffer);
        sprintf(buffer, "%li", suite->test_failures);
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "failures", BAD_CAST buffer);
        sprintf(buffer, "%.6f", difftime(suite->time.end, suite->time.start));
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "time", BAD_CAST buffer);
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "timestamp", BAD_CAST suite->timestamp);

        //der Vollständigkeit halber, weil ich nicht weiß, ob diese Informationen - die ich nicht habe - vorhanden sein müssen
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "errors", BAD_CAST "0");
        rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "skipped", BAD_CAST "0");
        int max = suite->tests;
        int j = 0;
        for (; j < max; ++j) {

            struct sput_test* test = &(suite->testsZgr[j]);

            rc = xmlTextWriterStartElement(writer, BAD_CAST "testcase");
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST test->name);
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "classname", BAD_CAST test->classname);
            sprintf(buffer, "%.6f", difftime(test->time.end, test->time.start));
            rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "time", BAD_CAST buffer);

            unsigned long k = 0;
            for (; k < test->checks && test->failure; ++k) {

                struct sput_check* check = &(test->checksZgr[k]);

                if (check->failure) {

                    rc = xmlTextWriterStartElement(writer, BAD_CAST "failure");
                    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "message", BAD_CAST check->cond);
                    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "type", BAD_CAST check->type);
                    sprintf(buffer, "%li", check->line);
                    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "line", BAD_CAST buffer);
                    rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", BAD_CAST check->name);
                    rc = xmlTextWriterEndElement(writer);

                    rc = xmlTextWriterStartElement(writer, BAD_CAST "system-out");
                    rc = xmlTextWriterWriteString(writer, check->cond);
                    rc = xmlTextWriterEndElement(writer);

                }

            }

            rc = xmlTextWriterEndElement(writer);
        }

        rc = xmlTextWriterEndElement(writer);

    }
    rc = xmlTextWriterEndElement(writer);
    rc = xmlTextWriterEndDocument(writer);
    xmlFreeTextWriter(writer);
}


#define sput_start_testing()                                               \
    do {                                                                   \
        memset(&__sput, 0, sizeof(__sput));                                \
        __sput.out         = stdout;                                       \
        __sput.overall.time.start  = time(NULL);                           \
        __sput.initialized = SPUT_INITIALIZED;                             \
    } while (0)

#define sput_leave_suite()                                                 \
    do {                                                                   \
        float failpls = 0.0f;                                              \
        _sput_die_unless_initialized();                                    \
        _sput_die_unless_suite_set();                                      \
        __sput.suite.time.end = time(NULL);                                \
        failpls = __sput.suite.checks ? (float)                            \
                  ((__sput.suite.nok * 100.0) / __sput.suite.checks) :     \
                  0.0f;                                                    \
        fprintf(__sput.out,                                                \
                "\n--> %lu check(s), %lu ok, %lu failed (%.2f%%)\n",       \
                __sput.suite.checks, __sput.suite.ok, __sput.suite.nok,    \
                failpls);                                                  \
        __sput.overall.checks += __sput.suite.checks;                      \
        __sput.overall.ok     += __sput.suite.ok;                          \
        __sput.overall.nok    += __sput.suite.nok;                         \
        sput_suite_sichern();                                              \
        memset(&__sput.suite, 0, sizeof(__sput.suite));                    \
    } while (0)


#define sput_get_return_value()                                            \
    (__sput.overall.nok > 0 ? EXIT_FAILURE : EXIT_SUCCESS)


#define sput_enter_suite(_name)                                            \
    do {                                                                   \
        _sput_die_unless_initialized();                                    \
        if (__sput.suite.name)                                             \
        {                                                                  \
            sput_leave_suite();                                            \
        }                                                                  \
        memset(&__sput.suite, 0, sizeof(struct sput_suite));               \
        __sput.suite.name = _name != NULL ?                                \
                            _name : SPUT_DEFAULT_SUITE_NAME;               \
        __sput.suite.nr = ++__sput.overall.suites;                         \
        getDateTime(__sput.suite.timestamp);                               \
        __sput.suite.time.start = time(NULL);                              \
        fprintf(__sput.out, "\n== Entering suite #%lu, \"%s\" ==\n\n",     \
                __sput.suite.nr, __sput.suite.name);                       \
} while (0)


#define sput_finish_testing()                                              \
    do {                                                                   \
        float failpft = 0.0f;                                              \
        _sput_die_unless_initialized();                                    \
        if (__sput.suite.name)                                             \
        {                                                                  \
            sput_leave_suite();                                            \
        }                                                                  \
        failpft = __sput.overall.checks ? (float)                          \
                  ((__sput.overall.nok * 100.0) / __sput.overall.checks) : \
                  0.0f;                                                    \
        __sput.overall.time.end = time(NULL);                              \
        fprintf(__sput.out,                                                \
                "\n==> %lu check(s) in %lu suite(s) finished after %.2f "  \
                "second(s),\n"                                             \
                "    %lu succeeded, %lu failed (%.2f%%)\n"                 \
                "\n[%s]\n",                                                \
                __sput.overall.checks, __sput.overall.suites,              \
                difftime(__sput.overall.time.end, __sput.overall.time.start),\
                __sput.overall.ok, __sput.overall.nok, failpft,            \
                (sput_get_return_value() == EXIT_SUCCESS) ?                \
                "SUCCESS" : "FAILURE");                                    \
} while (0)


#define sput_set_output_stream(_fp)                                        \
    do {                                                                   \
        __sput.out = _fp != NULL ? _fp : stdout;                           \
    } while (0)


#define sput_fail_if(_cond, _name)                                         \
    do {                                                                   \
        _sput_die_unless_initialized();                                    \
        _sput_die_unless_suite_set();                                      \
        _sput_die_unless_test_set();                                       \
        __sput.check.name = _name != NULL ?                                \
                            _name : SPUT_DEFAULT_CHECK_NAME;               \
        __sput.check.line = __LINE__;                                      \
        __sput.check.cond = #_cond;                                        \
        __sput.check.type = "fail-if";                                     \
        __sput.test.nr++;                                                  \
        __sput.suite.checks++;                                             \
        __sput.test.checks++;                                              \
        sput_check_sichern();                                              \
        if ((_cond))                                                       \
        {                                                                  \
            _sput_check_failed();                                          \
        }                                                                  \
        else                                                               \
        {                                                                  \
            _sput_check_succeeded();                                       \
        }                                                                  \
    } while (0)


#define sput_fail_unless(_cond, _name)                                     \
    do {                                                                   \
        _sput_die_unless_initialized();                                    \
        _sput_die_unless_suite_set();                                      \
        _sput_die_unless_test_set();                                       \
        __sput.check.name = _name != NULL ?                                \
                            _name : SPUT_DEFAULT_CHECK_NAME;               \
        __sput.check.line = __LINE__;                                      \
        __sput.check.cond = #_cond;                                        \
        __sput.check.type = "fail-unless";                                 \
        __sput.test.nr++;                                                  \
        __sput.suite.checks++;                                             \
        __sput.test.checks++;                                              \
        sput_check_sichern();                                              \
        if (! (_cond))                                                     \
        {                                                                  \
            _sput_check_failed();                                          \
        }                                                                  \
        else                                                               \
        {                                                                  \
            _sput_check_succeeded();                                       \
        }                                                                  \
    } while (0)


#define sput_run_test(_func, _classname)                                   \
    do {                                                                   \
        _sput_die_unless_initialized();                                    \
        _sput_die_unless_suite_set();                                      \
        memset(&__sput.test, 0, sizeof(struct sput_test));                 \
        __sput.test.name = #_func;                                         \
        __sput.test.classname = _classname;                               \
        __sput.test.time.start = time(NULL);                               \
        _func();                                                           \
        __sput.test.time.end = time(NULL);                                 \
        __sput.suite.tests++;                                              \
        if(__sput.test.failure)                                            \
            __sput.suite.test_failures++;                                  \
        sput_test_sichern();                                               \
    } while (0)

#ifdef __cplusplus
}
#endif


#endif /* HAVE_SPUT_H */