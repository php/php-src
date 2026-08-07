--TEST--
Bug #78697: ReflectionClass::implementsInterface - inaccurate error message with traits
--FILE--
<?php
trait T {}

try {
    (new ReflectionClass(new stdClass))->implementsInterface(T::class);
} catch (ReflectionException $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
ReflectionException: T is not an interface
