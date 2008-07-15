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
--FILE--
<?php

ob_start();

/* 
 * Prototype : array session_get_cookie_params(void)
 * Description : Get the session cookie parameters
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_get_cookie_params() : basic functionality ***\n";

var_dump(session_get_cookie_params());
var_dump(session_set_cookie_params(3600, "/path", "blah", FALSE, FALSE));
var_dump(session_get_cookie_params());
var_dump(session_set_cookie_params(1234567890, "/guff", "foo", TRUE, TRUE));
var_dump(session_get_cookie_params());

echo "Done";
ob_end_flush();
?>
--EXPECTF--
*** Testing session_get_cookie_params() : basic functionality ***
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
NULL
array(5) {
  [u"lifetime"]=>
  int(3600)
  [u"path"]=>
  unicode(5) "/path"
  [u"domain"]=>
  unicode(4) "blah"
  [u"secure"]=>
  bool(false)
  [u"httponly"]=>
  bool(false)
}
NULL
array(5) {
  [u"lifetime"]=>
  int(1234567890)
  [u"path"]=>
  unicode(5) "/guff"
  [u"domain"]=>
  unicode(3) "foo"
  [u"secure"]=>
  bool(true)
  [u"httponly"]=>
  bool(true)
}
Done
