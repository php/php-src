--TEST--
ReflectionClass::makeLazyGhost() no trampoline leak
--FILE--
<?php
class Helper {
    public function __call($name, $args) {}
}

try {
    (new ReflectionClass('ReflectionClass'))->newLazyGhost([new Helper, 'foo']);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot make instance of internal class lazy: ReflectionClass is internal
