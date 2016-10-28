--TEST--
Test session_set_serializer() function : interface functionality
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
 * Prototype : bool session_set_serializer(SessionSerializerInterface $obj)
 * Description : Sets user-level session serializer functions
 * Source code : ext/session/session.c, ext/session/serializer_user.c
 */

echo "*** Testing session_set_serializer() : interface functionality ***\n";

class MySerializer {

	function encode($array) {
		echo "encoded: ". serialize($array) ."\n";
		return serialize($array);
	}

	function decode($string) {
		echo "decoding: ". ($string) . "\n";
		return unserialize($string) ?: array();
	}
}

$handler = new MySerializer;

var_dump(session_set_serializer($handler));
// Shound't reach here
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
*** Testing session_set_serializer() : interface functionality ***

Warning: session_set_serializer() expects parameter 1 to be SessionSerializerInterface, object given in %s on line %d

Recoverable fatal error: session_set_serializer(): Only SessionSerializerInterface object can be used as session serializer object in %s on line 28

