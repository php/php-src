--TEST--
Test session_set_save_handler() function : basic functionality
--INI--
session.save_path=
session.name=PHPSESSID
session.gc_probability=0
session.save_handler=files
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : basic functionality ***\n";

require_once "save_handler.inc";
var_dump(session_module_name());
var_dump(session_module_name(FALSE));
var_dump(session_module_name("blah"));
var_dump(session_module_name("foo"));

$path = __DIR__ . '/session_set_save_handler_basic';
@mkdir($path);
session_save_path($path);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");

session_start();
$session_id = session_id();
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
$_SESSION['Bar'] = 'Foo';
session_write_close();

echo "Garbage collection..\n";
session_id($session_id);
session_start();
var_dump(session_gc());
session_write_close();

echo "Cleanup..\n";
session_id($session_id);
session_start();
session_destroy();

ob_end_flush();
rmdir($path);
?>
--EXPECTF--
*** Testing session_set_save_handler() : basic functionality ***
string(%d) "%s"

Warning: session_module_name(): Session handler module "" cannot be found in %s on line %d
bool(false)

Warning: session_module_name(): Session handler module "blah" cannot be found in %s on line %d
bool(false)

Warning: session_module_name(): Session handler module "foo" cannot be found in %s on line %d
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
Write [%s,%s,Blah|s:12:"Hello World!";Foo|b:0;Guff|i:1234567890;]
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
Write [%s,%s,Blah|s:12:"Hello World!";Foo|b:0;Guff|i:1234567890;Bar|s:3:"Foo";]
Close [%s,PHPSESSID]
Garbage collection..
Open [%s,PHPSESSID]
Read [%s,%s]
int(0)
Write [%s,%s,Blah|s:12:"Hello World!";Foo|b:0;Guff|i:1234567890;Bar|s:3:"Foo";]
Close [%s,PHPSESSID]
Cleanup..
Open [%s,PHPSESSID]
Read [%s,%s]
Destroy [%s,%s]
Close [%s,PHPSESSID]
