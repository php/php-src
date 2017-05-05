--TEST--
Test session_set_serializer() function : variation4
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
	die("Died in decode()\n");
	return unserialize($string) ?: array();
}

var_dump(session_set_serializer("encode", "decode"));

// Shouldn't reach here
var_dump(session_start());
$session_id = session_id();
var_dump($session_id, session_commit());

echo "Starting session again..!\n";
session_id($session_id);
var_dump(session_start());
var_dump(session_commit());

echo "Cleanup..\n";
session_id($session_id);
var_dump(session_start());
session_destroy();

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_serializer() : basic functionality ***
bool(true)
decoding: 
Died in decode()
encoded: a:0:{}
