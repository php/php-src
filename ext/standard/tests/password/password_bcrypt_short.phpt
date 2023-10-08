--TEST--
Test that password_hash() does not overread buffers when a short hash is passed
--FILE--
<?php
var_dump(password_verify("foo", '$2'));
?>
--EXPECT--
bool(false)
