--TEST--
Test session_set_cookie_params() function : array parameter variation
--INI--
session.cookie_lifetime=0
session.cookie_path="/"
session.cookie_domain=""
session.cookie_secure=0
session.cookie_httponly=0
session.cookie_samesite=""
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_cookie_params() : array parameter variation ***\n";

// Invalid cases
try {
    session_set_cookie_params([]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    session_set_cookie_params(["unknown_key" => true, "secure_invalid" => true]);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(ini_get("session.cookie_secure"));
var_dump(ini_get("session.cookie_samesite"));
var_dump(session_set_cookie_params(["secure" => true, "samesite" => "please"]));
var_dump(ini_get("session.cookie_secure"));
var_dump(ini_get("session.cookie_samesite"));

var_dump(ini_get("session.cookie_lifetime"));
var_dump(session_set_cookie_params(["lifetime" => 42]));
var_dump(ini_get("session.cookie_lifetime"));

var_dump(ini_get("session.cookie_path"));

try {
    session_set_cookie_params(["path" => "newpath/"], "arg after options array");
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

var_dump(ini_get("session.cookie_path"));

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_cookie_params() : array parameter variation ***
session_set_cookie_params(): Argument #1 ($lifetime_or_options) must contain at least 1 valid key

Warning: session_set_cookie_params(): Argument #1 ($lifetime_or_options) contains an unrecognized key "unknown_key" in %s on line %d

Warning: session_set_cookie_params(): Argument #1 ($lifetime_or_options) contains an unrecognized key "secure_invalid" in %s on line %d
session_set_cookie_params(): Argument #1 ($lifetime_or_options) must contain at least 1 valid key
string(1) "0"
string(0) ""
bool(true)
string(1) "1"
string(6) "please"
string(1) "0"
bool(true)
string(2) "42"
string(1) "/"
session_set_cookie_params(): Argument #2 ($path) must be null when argument #1 ($lifetime_or_options) is an array
string(1) "/"
Done
