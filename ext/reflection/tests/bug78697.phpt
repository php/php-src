--TEST--
Bug #78697: ReflectionClass::implementsInterface - inaccurate error message with traits
--FILE--
<?php
trait T {}

try {
    (new ReflectionClass(new stdClass))->implementsInterface(T::class);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
ReflectionException: T is not an interface
