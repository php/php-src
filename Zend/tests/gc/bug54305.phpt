--TEST--
Bug #54305 (Crash in gc_remove_zval_from_buffer)
--FILE--
<?php
class TestClass {
    public function methodWithArgs($a, $b) {
    }
}
abstract class AbstractClass {
}
$methodWithArgs = new ReflectionMethod('TestClass', 'methodWithArgs');
try {
    echo $methodWithArgs++;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: Cannot increment ReflectionMethod
