--TEST--
ReflectionProperty has an isImmutable method.
--FILE--
<?php
var_dump(method_exists(ReflectionProperty::class, 'isImmutable'));
?>
--EXPECT--

bool(true)
