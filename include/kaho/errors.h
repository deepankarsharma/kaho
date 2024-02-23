#pragma once
#include <QDebug>

#define RETURN_ON_ERROR(condition, return_value, error_message) \
    do { \
        if (condition) { \
            qDebug() << error_message; \
            return return_value; \
        } \
    } while (0)

namespace kaho {
    enum class ErrorCode {
        OK = 0,  // For representing success

        // Common Error Categories
        CANCELLED,
        UNKNOWN,
        INVALID_ARGUMENT,
        DEADLINE_EXCEEDED,
        NOT_FOUND,
        ALREADY_EXISTS,
        PERMISSION_DENIED,
        RESOURCE_EXHAUSTED,
        FAILED_PRECONDITION,
        ABORTED,
        OUT_OF_RANGE,
        UNIMPLEMENTED,
        INTERNAL,
        UNAVAILABLE,
        DATA_LOSS,
        UNAUTHENTICATED,
        IO_FAILED

    };

    bool is_ok(ErrorCode code);
    bool is_error(ErrorCode code);
}
