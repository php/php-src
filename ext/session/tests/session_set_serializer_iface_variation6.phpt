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

class MySerializer implements SessionSerializerInterface {

	function encode($array) {
		echo "encoded: ". serialize($array) ."\n";
		return serialize($array);
	}

	function decode($string) {
		echo "decoding: ". ($string) . "\n";
		return unserialize($string);
	}
}

$handler = new MySerializer;

var_dump(session_set_serializer($handler));

$data = array('foo'=>'bar');
$encoded = $handler->encode($data);
$decoded = $handler->decode($encoded);
var_dump($encoded, $decoded);

ob_end_flush();
?>
--EXPECTF--
*** Testing session_set_serializer() : interface functionality ***
bool(true)
encoded: a:1:{s:3:"foo";s:3:"bar";}
decoding: a:1:{s:3:"foo";s:3:"bar";}
string(26) "a:1:{s:3:"foo";s:3:"bar";}"
array(1) {
  ["foo"]=>
  string(3) "bar"
}