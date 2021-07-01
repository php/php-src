--TEST--
Test ini_set() for session : basic functionality
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_path=
session.name="PHPSESSID"
session.save_handler="files"
session.auto_start="0"
session.gc_probability="1"
session.gc_divisor="100"
session.gc_maxlifetime="1440"
session.serialize_handler="php"
session.cookie_path="/"
session.cookie_domain=""
session.cookie_secure="0"
session.cookie_httponly="0"
session.use_cookies="1"
session.use_only_cookies="1"
session.use_strict_mode="0"
session.referer_check=""
session.cache_limiter="nocache"
session.cache_expire="180"
session.use_trans_sid="0"
session.sid_length="32"
session.sid_bits_per_character="4"
session.lazy_write="1"
--FILE--
<?php
ob_start();

/*
 * Prototype : string ini_set(string $name, string $value)
 * Description : Set session ini
 * Source code : ext/session/session.c
 */

echo "*** Testing ini_set() for session ini: basic functionality ***\n";

var_dump(ini_set("session.save_path", ""));
var_dump(ini_set("session.name", "PHPSESSID"));
var_dump(ini_set("session.save_handler", "files"));
var_dump(ini_set("session.auto_start", "0"));
var_dump(ini_set("session.gc_probability", "1"));
var_dump(ini_set("session.gc_divisor", "100"));
var_dump(ini_set("session.gc_maxlifetime", "1440"));
var_dump(ini_set("session.serialize_handler", "php"));
var_dump(ini_set("session.cookie_path", "/"));
var_dump(ini_set("session.cookie_domain", ""));
var_dump(ini_set("session.cookie_secure", "0"));
var_dump(ini_set("session.cookie_httponly", "0"));
var_dump(ini_set("session.use_cookies", "1"));
var_dump(ini_set("session.use_only_cookies", "1"));
var_dump(ini_set("session.use_strict_mode", "0"));
var_dump(ini_set("session.referer_check", ""));
var_dump(ini_set("session.cache_limiter", "nocache"));
var_dump(ini_set("session.cache_expire", "180"));
var_dump(ini_set("session.use_trans_sid", "0"));
var_dump(ini_set("session.sid_length", "32"));
var_dump(ini_set("session.sid_bits_per_character", "4"));
var_dump(ini_set("session.lazy_write", "1"));

session_start();
var_dump("session started");

var_dump(ini_set("session.save_path", ""));
var_dump(ini_set("session.name", "PHPSESSID"));
var_dump(ini_set("session.save_handler", "files"));
var_dump(ini_set("session.auto_start", "0"));
var_dump(ini_set("session.gc_probability", "1"));
var_dump(ini_set("session.gc_divisor", "100"));
var_dump(ini_set("session.gc_maxlifetime", "1440"));
var_dump(ini_set("session.serialize_handler", "php"));
var_dump(ini_set("session.cookie_path", "/"));
var_dump(ini_set("session.cookie_domain", ""));
var_dump(ini_set("session.cookie_secure", "0"));
var_dump(ini_set("session.cookie_httponly", "0"));
var_dump(ini_set("session.use_cookies", "1"));
var_dump(ini_set("session.use_only_cookies", "1"));
var_dump(ini_set("session.use_strict_mode", "0"));
var_dump(ini_set("session.referer_check", ""));
var_dump(ini_set("session.cache_limiter", "nocache"));
var_dump(ini_set("session.cache_expire", "180"));
var_dump(ini_set("session.use_trans_sid", "0"));
var_dump(ini_set("session.sid_length", "32"));
var_dump(ini_set("session.sid_bits_per_character", "4"));
var_dump(ini_set("session.lazy_write", "1"));


echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing ini_set() for session ini: basic functionality ***
string(0) ""
string(9) "PHPSESSID"
string(5) "files"
bool(false)
string(1) "1"
string(3) "100"
string(4) "1440"
string(3) "php"
string(1) "/"
string(0) ""
string(1) "0"
string(1) "0"
string(1) "1"
string(1) "1"
string(1) "0"
string(0) ""
string(7) "nocache"
string(3) "180"
string(1) "0"
string(2) "32"
string(1) "4"
string(1) "1"
string(15) "session started"

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)

Warning: ini_set(): Session ini settings cannot be changed when a session is active in %s on line %d
bool(false)
Done
