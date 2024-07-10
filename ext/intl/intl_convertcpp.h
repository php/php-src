/*
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | https://www.php.net/license/3_01.txt                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Gustavo Lopes <cataphract@php.net>                          |
   +----------------------------------------------------------------------+
*/

#ifndef INTL_CONVERTCPP_H
#define INTL_CONVERTCPP_H

#ifndef __cplusplus
#error Should be included only in C++ Files
#endif

#include <unicode/unistr.h>
#include <zend_types.h>

using icu::UnicodeString;

int intl_stringFromChar(UnicodeString &ret, char *str, size_t str_len, UErrorCode *status);

zend_string* intl_charFromString(const UnicodeString &from, UErrorCode *status);

/**
 * Utility class to act as a cleaner when an instance
 * uses the expression when it goes out of its scope
 * avoiding to needs handle all errors in all code paths
 *
 * note that template auto deduction is supported
 * only from C++17, still need to be explicit
 * at instantiation's time.
 */
template<typename T>
class intl_cleaner {
    T expr;
    bool useexpr{true};
public:
    intl_cleaner(T& expr_) :
      expr(std::move(expr_)) {}
    intl_cleaner(const intl_cleaner& o) = delete;
    intl_cleaner& operator=(const intl_cleaner& o) = delete;
    // for the assignment constructors with std::move semantics
    // we invalidate the original.
    intl_cleaner(intl_cleaner&& o) :
      expr(std::move(o.expr)),
      useexpr(o.useexpr) {
        o.useexpr = false;
    }
    intl_cleaner& operator=(const intl_cleaner&& o) {
        if (&o == this)
            return *this;
        this->~intl_cleaner();
        new (this)intl_cleaner(std::move(o));
        return *this;
    }
    ~intl_cleaner() {
        if (useexpr)
            expr();
    }
};
#endif /* INTL_CONVERTCPP_H */
