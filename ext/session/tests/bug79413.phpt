--TEST--
Bug #79413 (session_create_id() fails for active sessions)
--EXTENSIONS--
session
--FILE--
<?php
session_start();
$old = session_id();
$new = session_create_id();
var_dump($new !== $old);
?>
--EXPECT--
bool(true)
