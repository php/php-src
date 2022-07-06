--TEST--
Bug #79413 (session_create_id() fails for active sessions)
--SKIPIF--
<?php
if (!extension_loaded('session')) die('skip session extension not available');
?>
--FILE--
<?php
session_start();
$old = session_id();
$new = session_create_id();
var_dump($new !== $old);
?>
--EXPECT--
bool(true)
