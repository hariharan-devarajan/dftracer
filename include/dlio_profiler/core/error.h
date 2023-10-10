//
// Created by haridev on 10/5/23.
//

#ifndef DLIO_PROFILER_ERROR_H
#define DLIO_PROFILER_ERROR_H

struct ErrorCode {
    const char *code;
    const char *message;
};

// Main error codes
const ErrorCode SUCCESS = {"1000", "Operation Successful"};
const ErrorCode FAILURE = {"1001", "Internal Failure"};

// Invalid API calls
const ErrorCode UNKNOWN_PROFILER_TYPE = {"1002", "Code 1002: Unknown profiler type %d"};

// Invalid configurations
const ErrorCode UNDEFINED_DATA_DIR = {"2001",
                                      "Code 2001: Data dirs not defined. Please define env variable DLIO_PROFILER_DATA_DIR"};
const ErrorCode UNDEFINED_LOG_FILE = {"2002",
                                      "Code 2002: log file not defined. Please define env variable DLIO_PROFILER_LOG_FILE"};

#endif //DLIO_PROFILER_ERROR_H
