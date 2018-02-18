--TEST--
ReflectionClass has an isImmutable method.
--FILE--
<?php
var_dump(method_exists(ReflectionClass::class, 'isImmutable'));
?>
--EXPECT--

bool(true)
