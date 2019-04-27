--TEST--
Test session_status() function : disabled
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.save_handler=non-existent
--FILE--
<?php

echo "*** Testing session_status() : disabled\n";

var_dump(session_status() == PHP_SESSION_DISABLED);

?>
--EXPECT--
*** Testing session_status() : disabled
bool(true)
