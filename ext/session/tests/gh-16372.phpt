--TEST--
GH-16372: Mention where headers were already sent if session_start fails
--EXTENSIONS--
session
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php

header("X-PHP-Test: test");

echo "Sent headers\n";

session_start();
?>
--EXPECTF--
Sent headers

Warning: session_start(): Session cannot be started after headers have already been sent (sent from %s on line %d) in %s on line %d
