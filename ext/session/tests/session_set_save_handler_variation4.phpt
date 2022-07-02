--TEST--
Test session_set_save_handler() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.gc_probability=1
session.gc_divisor=1
session.gc_maxlifetime=0
session.save_path=
session.name=PHPSESSID
session.save_handler=files
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : variation ***\n";

function noisy_gc($maxlifetime) {
    echo("GC [".$maxlifetime."]\n");
    echo gc($maxlifetime)." deleted\n";
    return true;
}

require_once "save_handler.inc";
$path = __DIR__ . '/session_set_save_handler_variation4';
@mkdir($path);
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
rmdir($path);
?>
--EXPECTF--
*** Testing session_set_save_handler() : variation ***
Open [%s,PHPSESSID]
Read [%s,%s]
GC [0]
1 deleted
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
bool(true)
Open [%s,PHPSESSID]
Read [%s,%s]
GC [0]
1 deleted
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
