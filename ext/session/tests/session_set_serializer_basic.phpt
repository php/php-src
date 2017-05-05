--TEST--
Test session_set_serializer() function : basic functionality
--INI--
session.use_strict_mode=0
session.save_path=
session.name=PHPSESSID
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

/*
 * Prototype : bool session_set_serializer(callback $encode, callback $decode)
 * Description : Sets user-level session serializer functions
 * Source code : ext/session/session.c, ext/session/serializer_user.c
 */

echo "*** Testing session_set_serializer() : basic functionality ***\n";

function encode($array) {
	echo "encoded: ". serialize($array) ."\n";
	return serialize($array);
}

function decode($string) {
	echo "decoding: ". ($string) . "\n";
	return unserialize($string) ?: array();
}

$path = dirname(__FILE__);
session_save_path($path);
var_dump(session_set_serializer("encode", "decode"));

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
session_start();
var_dump($_SESSION);
$_SESSION['Bar'] = 'Foo';
session_write_close();

echo "Cleanup..\n";
session_id($session_id);
session_start();
session_destroy();

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_serializer() : basic functionality ***
bool(true)
decoding: 
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
encoded: a:3:{s:4:"Blah";s:12:"Hello World!";s:3:"Foo";b:0;s:4:"Guff";i:1234567890;}
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
Starting session again..!
decoding: a:3:{s:4:"Blah";s:12:"Hello World!";s:3:"Foo";b:0;s:4:"Guff";i:1234567890;}
array(3) {
  ["Blah"]=>
  string(12) "Hello World!"
  ["Foo"]=>
  bool(false)
  ["Guff"]=>
  int(1234567890)
}
encoded: a:4:{s:4:"Blah";s:12:"Hello World!";s:3:"Foo";b:0;s:4:"Guff";i:1234567890;s:3:"Bar";s:3:"Foo";}
Cleanup..
decoding: a:4:{s:4:"Blah";s:12:"Hello World!";s:3:"Foo";b:0;s:4:"Guff";i:1234567890;s:3:"Bar";s:3:"Foo";}
