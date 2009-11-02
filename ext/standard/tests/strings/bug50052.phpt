--TEST--
Bug #20934 (html_entity_decode() crash when "" is passed)
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
