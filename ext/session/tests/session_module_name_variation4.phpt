--TEST--
Test session_module_name() function : variation
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.gc_probability=1
session.gc_divisor=1
session.gc_maxlifetime=0
--FILE--
<?php

ob_start();

echo "*** Testing session_module_name() : variation ***\n";

require_once "save_handler.inc";
$path = __DIR__;
$path = __DIR__ . '/session_module_name_variation4';
@mkdir($path);
session_save_path($path);
session_module_name("files");

session_start();
$_SESSION["Blah"] = "Hello World!";
$_SESSION["Foo"] = FALSE;
$_SESSION["Guff"] = 1234567890;
var_dump($_SESSION);
$oldsession = $_SESSION;

var_dump(session_write_close());
session_start();
// the session may have been GC'd or not; we accept either outcome
var_dump($_SESSION === $oldsession || $_SESSION === []);
var_dump(session_destroy());
session_start();
var_dump($_SESSION);
var_dump(session_destroy());

ob_end_flush();
rmdir($path);
?>
--EXPECT--
*** Testing session_module_name() : variation ***
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
bool(true)
bool(true)
bool(true)
array(0) {
}
bool(true)
