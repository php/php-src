--TEST--
Bug #50052 (Different Hashes on Windows and Linux on wrong Salt size)
--FILE--
<?php
$salt = '$1$f+uslYF01$';
$password = 'test';
echo $salt . "\n";
echo crypt($password,$salt) . "\n";
?>
--EXPECT--
$1$f+uslYF01$
$1$f+uslYF0$orVloNmKSLvOeswusE0bY.
