--TEST--
Test normal operation of random_bytes()
--FILE--
<?php
//-=-=-=-

var_dump(strlen(bin2hex(random_bytes(16))));

var_dump(is_string(random_bytes(10)));

var_dump(is_string(random_bytes(257)));

?>
--EXPECT--
int(32)
bool(true)
bool(true)
