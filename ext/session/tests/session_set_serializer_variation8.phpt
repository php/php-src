--TEST--
Test session_set_serializer() function : invalid parameters
--INI--
session.save_path=
session.name=PHPSESSID
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

echo "*** session_set_serializer() function : invalid parameters ***\n";

function encode($array) {
	echo "encoded: ". serialize($array) ."\n";
	return serialize($array);
}

function decode($string) {
	echo "decoding: ". ($string) . "\n";
	return unserialize($string) ?: array();
}

function bad_encode($array) {
	return array(1,2,3);
}

function bad_decode($string) {
	return 123;
}

var_dump(session_set_serializer("encode", "bad_decode"));

require_once "save_handler.inc";
$path = dirname(__FILE__);
session_save_path($path);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");

session_start();
$session_id = session_id();
var_dump($_SESSION);

session_write_close();
session_unset();

echo "Starting session again..!\n";
session_id($session_id);
session_set_save_handler("open", "close", "read", "write", "destroy", "gc");
session_start();
var_dump($_SESSION);
session_write_close();

echo "Cleanup..\n";
session_id($session_id);
session_start();
session_destroy();

ob_end_flush();
?>
--CLEAN--
<?php
foreach(glob('ext/session/tests/session_test_*') as $file) {
	@unlink($file);
}
?>
--EXPECTF--
*** session_set_serializer() function : invalid parameters ***
bool(true)

Open [%s,PHPSESSID]
Read [%s,%s]

Recoverable fatal error: session_start(): User session decode function must return array in %s on line 38
encoded: a:0:{}
Write [%s,%s,a:0:{}]
Close [%s,PHPSESSID]