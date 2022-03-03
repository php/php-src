--TEST--
Test session_set_save_handler() : register session handler but don't start
--INI--
session.save_handler=files
session.name=PHPSESSID
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

ob_start();

echo "*** Testing session_set_save_handler() : register session handler but don't start  ***\n";

session_set_save_handler(new SessionHandler);
?>
--EXPECT--
*** Testing session_set_save_handler() : register session handler but don't start  ***
