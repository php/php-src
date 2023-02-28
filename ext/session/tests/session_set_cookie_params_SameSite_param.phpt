--TEST--
session_set_cookie_params() SameSite parameter tests
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

// Check type errors work
try {
    session_set_cookie_params(20, sameSite: new stdClass());
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

// Check ValueError when using SameSite::None and the cookie is not secure, see
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie/SameSite#samesitenone_requires_secure
try {
    session_set_cookie_params(20, sameSite: SameSite::None);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    session_set_cookie_params(20, sameSite: SameSite::None, secure: false);
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

// Check get argument count error when using an option array
try {
    session_set_cookie_params(['secure' => false], sameSite: SameSite::None);
} catch (\ArgumentCountError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

echo "*** Testing session_set_cookie_params(20, sameSite:) ***\n";
var_dump(session_set_cookie_params(20, sameSite: SameSite::None, secure: true));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(20, sameSite: SameSite::Lax, secure: true));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(20, sameSite: SameSite::Strict, secure: true));
var_dump(ini_get("session.cookie_samesite"));
// After session started
var_dump(session_start());
var_dump(session_set_cookie_params(20, sameSite: SameSite::None, secure: true));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(20, sameSite: SameSite::Lax, secure: true));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(20, sameSite: SameSite::Strict, secure: true));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
session_set_cookie_params(): Argument #6 ($sameSite) must be of type SameSite, stdClass given
session_set_cookie_params(): Argument #6 ($sameSite) can only be SameSite::None if argument #6 ($secure) is true
session_set_cookie_params(): Argument #6 ($sameSite) can only be SameSite::None if argument #6 ($secure) is true
session_set_cookie_params(): Expects exactly 1 arguments when argument #1 ($lifetime_or_options) is an array
*** Testing session_set_cookie_params(20, sameSite:) ***
bool(true)
string(4) "None"
bool(true)
string(3) "Lax"
bool(true)
string(6) "Strict"
bool(true)

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
string(6) "Strict"

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
string(6) "Strict"

Warning: session_set_cookie_params(): Session cookie parameters cannot be changed when a session is active in %s on line %d
bool(false)
Done
