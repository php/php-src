--TEST--
Test session_set_serializer() function : variation5
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
	return TRUE;
}

function decode($string) {
	echo "decoding: ". ($string) . "\n";
	return unserialize($string) ?: array();
}


var_dump(session_set_serializer("encode", "decode"));

var_dump(session_start());
$session_id = session_id();
// encode() is executed by session_commit().
// encode() returns invalid type and RSHUTDOWN tries to finish session.
// Therefore, ecode() is called twice.
var_dump($session_id, session_commit());

// Shouldn't reach here
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
bool(true)
encoded: a:0:{}

Recoverable fatal error: session_commit(): Session decode callback expects string or FALSE return value in %s on line 32
encoded: a:0:{}

Recoverable fatal error: Unknown: Session decode callback expects string or FALSE return value in Unknown on line 0
