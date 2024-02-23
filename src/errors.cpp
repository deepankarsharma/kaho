#include <kaho/errors.h>

namespace kaho {
    bool is_ok(ErrorCode code) {
        return code == ErrorCode::OK;
    }

    bool is_error(ErrorCode code) {
        return !is_ok(code);
    }
} // namespace kaho