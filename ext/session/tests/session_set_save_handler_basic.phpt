--TEST--
Test session_set_save_handler() function : basic functionality
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/* 
 * Prototype : bool session_set_save_handler(callback $open, callback $close, callback $read, callback $write, callback $destroy, callback $gc)
 * Description : Sets user-level session storage functions
 * Source code : ext/session/session.c 
 */

echo "*** Testing session_set_save_handler() : basic functionality ***\n";

require_once "save_handler.inc";
var_dump(session_module_name());
var_dump(session_module_name(FALSE));
var_dump(session_module_name("blah"));
var_dump(session_module_name("foo"));

$path = dirname(__FILE__);
session_save_path($path);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");

session_start();
$_SESSION["Blah"] = "Hello World!";
$_SESSION["Foo"] = FALSE;
$_SESSION["Guff"] = 1234567890;
var_dump($_SESSION);

session_write_close();
session_unset();
var_dump($_SESSION);

echo "Starting session again..!\n";
session_id($session_id);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
session_start();
var_dump($_SESSION);
session_write_close();

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_save_handler() : basic functionality ***

string(5) "files"

Warning: session_module_name(): Cannot find named PHP session module () in %s on line %d
bool(false)

Warning: session_module_name(): Cannot find named PHP session module (blah) in %s on line %d
bool(false)

Warning: session_module_name(): Cannot find named PHP session module (foo) in %s on line %d
bool(false)
Open [%s,PHPSESSID]
Read [%s,%s]
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Write [%s,%s,Blah|S:12:"Hello World!";Foo|b:0;Guff|i:1234567890;]
Close [%s,PHPSESSID]
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Starting session again..!
Open [%s,PHPSESSID]
Read [%s,%s]
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Write [%s,%s,Blah|S:12:"Hello World!";Foo|b:0;Guff|i:1234567890;]
Close [%s,PHPSESSID]
--UEXPECTF--
*** Testing session_set_save_handler() : basic functionality ***

unicode(5) "files"

Warning: session_module_name(): Cannot find named PHP session module () in %s on line %d
bool(false)

Warning: session_module_name(): Cannot find named PHP session module (blah) in %s on line %d
bool(false)

Warning: session_module_name(): Cannot find named PHP session module (foo) in %s on line %d
bool(false)
Open [%s,PHPSESSID]
Read [%s,%s]
array(3) {
  [u"Blah"]=>
  unicode(12) "Hello World!"
  [u"Foo"]=>
  bool(false)
  [u"Guff"]=>
  int(1234567890)
}
Write [%s,%s,Blah|U:12:"Hello World!";Foo|b:0;Guff|i:1234567890;]

Notice: fwrite(): %d character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
Close [%s,PHPSESSID]
array(3) {
  [u"Blah"]=>
  unicode(12) "Hello World!"
  [u"Foo"]=>
  bool(false)
  [u"Guff"]=>
  int(1234567890)
}
Starting session again..!
Open [%s,PHPSESSID]
Read [%s,%s]
array(3) {
  [u"Blah"]=>
  unicode(12) "Hello World!"
  [u"Foo"]=>
  bool(false)
  [u"Guff"]=>
  int(1234567890)
}
Write [%s,%s,Blah|U:12:"Hello World!";Foo|b:0;Guff|i:1234567890;]

Notice: fwrite(): %d character unicode buffer downcoded for binary stream runtime_encoding in %s on line %d
Close [%s,PHPSESSID]
