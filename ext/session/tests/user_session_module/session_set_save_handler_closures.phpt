--TEST--
Test session_set_save_handler() function : using closures as callbacks
--INI--
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--EXTENSIONS--
session
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : using closures as callbacks ***\n";

require_once "save_handler_closures.inc";
var_dump(session_module_name());
var_dump(session_module_name(FALSE));
var_dump(session_module_name("blah"));
var_dump(session_module_name("foo"));

$path = __DIR__ . '/session_set_save_handler_closures';
@mkdir($path);
session_save_path($path);
session_set_save_handler($open_closure, $close_closure, $read_closure, $write_closure, $destroy_closure, $gc_closure);

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
session_set_save_handler($open_closure, $close_closure, $read_closure, $write_closure, $destroy_closure, $gc_closure);
session_start();
$_SESSION['Bar'] = 'Foo';
var_dump($_SESSION);
session_write_close();

echo "Cleanup\n";
session_start();
session_destroy();

ob_end_flush();
@rmdir($path);
?>
--EXPECTF--
*** Testing session_set_save_handler() : using closures as callbacks ***
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
array(4) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
  ["Bar"]=>
  string(3) "Foo"
}
Write [%s,%s,Blah|s:12:"Hello World!";Foo|b:0;Guff|i:1234567890;Bar|s:3:"Foo";]
Close [%s,PHPSESSID]
Cleanup
Open [%s,PHPSESSID]
Read [%s,%s]
Destroy [%s,%s]
Close [%s,PHPSESSID]
