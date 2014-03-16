--TEST--
Test session_set_save_handler() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
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

function noisy_gc($maxlifetime) {
	echo("GC [".$maxlifetime."]\n");
	gc($maxlifetime);
}

require_once "save_handler.inc";
$path = dirname(__FILE__);
session_save_path($path);
session_set_save_handler("open", "close", "read", "write", "destroy", "noisy_gc");

session_start();
$_SESSION["Blah"] = "Hello World!";
$_SESSION["Foo"] = FALSE;
$_SESSION["Guff"] = 1234567890;
var_dump($_SESSION);
$session_id = session_id();
var_dump(session_write_close());

session_set_save_handler("open", "close", "read", "write", "destroy", "noisy_gc");
session_id($session_id);
session_start();
var_dump($_SESSION);
var_dump(session_destroy());

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : variation ***

Open [%s,PHPSESSID]
Read [%s,%s]
GC [0]
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
Read [%s,%s]
GC [0]
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Destroy [%s,%s]

Warning: unlink(%s): No such file or directory in %s on line %d
Close [%s,PHPSESSID]
bool(true)

