--TEST--
Test that the value of PASSWORD_DEFAULT matches PASSWORD_BCRYPT
--FILE--
<?php
echo PASSWORD_DEFAULT . "\n";
echo PASSWORD_BCRYPT . "\n";
?>
--EXPECT--
2y
2y
