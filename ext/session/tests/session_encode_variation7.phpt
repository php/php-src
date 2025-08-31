--TEST--
Test session_encode() function : variation
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.serialize_handler=php_binary
--FILE--
<?php

ob_start();

echo "*** Testing session_encode() : variation ***\n";

var_dump(session_start());
$_SESSION["foo"] = 1234567890;
$encoded = session_encode();
var_dump(base64_encode($encoded));
var_dump(session_destroy());

echo "Done";
ob_end_flush();
?>
--EXPECT--
*** Testing session_encode() : variation ***
bool(true)
string(24) "A2Zvb2k6MTIzNDU2Nzg5MDs="
bool(true)
Done
