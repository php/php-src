--TEST--
Locale::acceptFromHttp() resets intl error before argument parsing
--EXTENSIONS--
intl
--INI--
intl.error_level=0
intl.use_exceptions=0
--FILE--
<?php
foreach (['Locale::acceptFromHttp', 'locale_accept_from_http'] as $func) {
    $func(str_repeat('a', 200));
    var_dump(intl_get_error_code() === U_ILLEGAL_ARGUMENT_ERROR);

    try {
        $func('en', 123);
    } catch (\Throwable $e) {
        echo $e::class, "\n";
    }

    var_dump(intl_get_error_code() === U_ZERO_ERROR);
    var_dump(intl_get_error_message());
}
?>
--EXPECT--
bool(true)
TypeError
bool(true)
string(12) "U_ZERO_ERROR"
bool(true)
TypeError
bool(true)
string(12) "U_ZERO_ERROR"
