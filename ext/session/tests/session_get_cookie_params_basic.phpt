--TEST--
Test session_get_cookie_params() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.cookie_lifetime=0
session.cookie_path="/"
session.cookie_domain=""
session.cookie_secure=0
session.cookie_httponly=0
session.cookie_samesite=""
--FILE--
<?php

ob_start();

echo "*** Testing session_get_cookie_params() : basic functionality ***\n";

var_dump(session_get_cookie_params());
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, FALSE));
var_dump(session_get_cookie_params());
var_dump(session_set_cookie_params(1234567890, "/guff", "foo", TRUE, TRUE));
var_dump(session_get_cookie_params());
var_dump(session_set_cookie_params([
  "lifetime" => 123,
  "path" => "/bar",
  "domain" => "baz",
  "secure" => FALSE,
  "httponly" => FALSE,
  "samesite" => "please"]));
var_dump(session_get_cookie_params());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_get_cookie_params() : basic functionality ***
array(6) {
  ["lifetime"]=>
  int(0)
  ["path"]=>
  string(1) "/"
  ["domain"]=>
  string(0) ""
  ["secure"]=>
  bool(false)
  ["httponly"]=>
  bool(false)
  ["samesite"]=>
  string(0) ""
}
bool(true)
array(6) {
  ["lifetime"]=>
  int(3600)
  ["path"]=>
  string(5) "/path"
  ["domain"]=>
  string(4) "blah"
  ["secure"]=>
  bool(false)
  ["httponly"]=>
  bool(false)
  ["samesite"]=>
  string(0) ""
}
bool(true)
array(6) {
  ["lifetime"]=>
  int(1234567890)
  ["path"]=>
  string(5) "/guff"
  ["domain"]=>
  string(3) "foo"
  ["secure"]=>
  bool(true)
  ["httponly"]=>
  bool(true)
  ["samesite"]=>
  string(0) ""
}
bool(true)
array(6) {
  ["lifetime"]=>
  int(123)
  ["path"]=>
  string(4) "/bar"
  ["domain"]=>
  string(3) "baz"
  ["secure"]=>
  bool(false)
  ["httponly"]=>
  bool(false)
  ["samesite"]=>
  string(6) "please"
}
Done
