--TEST--
INI value from env var
--ENV--
PHP_DISABLE_CLASSES=NotARealClass
--INI--
disable_classes=${PHP_DISABLE_CLASSES}
--FILE--
<?php
var_dump(ini_get('disable_classes'));
?>
--EXPECT--
string(13) "NotARealClass"
