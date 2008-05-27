--TEST--
Test session_get_cookie_params() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : array session_get_cookie_params(void)
 * Description : Get the session cookie parameters
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_get_cookie_params() : variation ***\n";

var_dump(session_get_cookie_params());
ini_set("session.cookie_lifetime", 3600);
var_dump(session_get_cookie_params());
ini_set("session.cookie_path", "/path");
var_dump(session_get_cookie_params());
ini_set("session.cookie_domain", "foo");
var_dump(session_get_cookie_params());
ini_set("session.cookie_secure", TRUE);
var_dump(session_get_cookie_params());
ini_set("session.cookie_httponly", TRUE);
var_dump(session_get_cookie_params());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_get_cookie_params() : variation ***
array(5) {
  [u"lifetime"]=>
  int(0)
  [u"path"]=>
  unicode(1) "/"
  [u"domain"]=>
  unicode(0) ""
  [u"secure"]=>
  bool(false)
  [u"httponly"]=>
  bool(false)
}
array(5) {
  [u"lifetime"]=>
  int(3600)
  [u"path"]=>
  unicode(1) "/"
  [u"domain"]=>
  unicode(0) ""
  [u"secure"]=>
  bool(false)
  [u"httponly"]=>
  bool(false)
}
array(5) {
  [u"lifetime"]=>
  int(3600)
  [u"path"]=>
  unicode(5) "/path"
  [u"domain"]=>
  unicode(0) ""
  [u"secure"]=>
  bool(false)
  [u"httponly"]=>
  bool(false)
}
array(5) {
  [u"lifetime"]=>
  int(3600)
  [u"path"]=>
  unicode(5) "/path"
  [u"domain"]=>
  unicode(3) "foo"
  [u"secure"]=>
  bool(false)
  [u"httponly"]=>
  bool(false)
}
array(5) {
  [u"lifetime"]=>
  int(3600)
  [u"path"]=>
  unicode(5) "/path"
  [u"domain"]=>
  unicode(3) "foo"
  [u"secure"]=>
  bool(true)
  [u"httponly"]=>
  bool(false)
}
array(5) {
  [u"lifetime"]=>
  int(3600)
  [u"path"]=>
  unicode(5) "/path"
  [u"domain"]=>
  unicode(3) "foo"
  [u"secure"]=>
  bool(true)
  [u"httponly"]=>
  bool(true)
}
Done

