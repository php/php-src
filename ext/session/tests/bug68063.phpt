--TEST--
Bug #68063 (Empty session IDs do still start sessions)
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
--FILE--
<?php
// Could also be set with a cookie like "PHPSESSID=; path=/"
session_id('');

// Will still start the session and return true
var_dump(session_start());

// Returns an empty string
var_dump(session_id());
?>
--EXPECTF--
Warning: session_start(): Cannot start session with empty session ID in %s on line %d
bool(false)
string(0) ""
