--TEST--
Test session_set_save_handler() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_strict_mode=1
session.gc_probability=1
session.gc_divisor=1
session.gc_maxlifetime=0
session.save_path=
session.name=PHPSESSID
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_set_save_handler(callback $open, callback $close, callback $read, callback $write, callback $destroy, callback $gc)
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c
 */

echo "*** Testing session_set_save_handler() : variation ***\n";

require_once "save_handler.inc";
$path = dirname(__FILE__);
session_save_path($path);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc", "create_sid", "validate_sid", "update", "feature");

session_start();
$_SESSION["Blah"] = "Hello World!";
$_SESSION["Foo"] = FALSE;
$_SESSION["Guff"] = 1234567890;
var_dump($_SESSION);
$session_id = session_id();
var_dump(session_write_close());

session_set_save_handler("open", "close", "read", "write", "destroy", "gc", "create_sid", "validate_sid", "update", "feature");
session_id($session_id);
session_start(['lazy_write'=>TRUE]);
var_dump($_SESSION);
session_commit();

// Cleanup
session_id($session_id);
session_start();
session_destroy();

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : variation ***

Open [%s,PHPSESSID]
CreateID [PHPT-%s]
ValidateID [%s,PHPT-%s]
Read [%s,%s]
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Write [%s,%s,Blah|s:12:"Hello World!";Foo|b:0;Guff|i:1234567890;]
Close [%s,PHPSESSID]
NULL
Open [%s,PHPSESSID]
ValidateID [%s,PHPT-%s]
Read [%s,%s]
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Update [%s,PHPT-%s]
Close [%s,PHPSESSID]
Open [%s,PHPSESSID]
ValidateID [%s,PHPT-%s]
Read [%s,PHPT-%s]
Destroy [%s,PHPT-%s]

Warning: unlink(%s/session_test_PHPT-%s): No such file or directory in %s/save_handler.inc on line 45
Close [%s,PHPSESSID]
