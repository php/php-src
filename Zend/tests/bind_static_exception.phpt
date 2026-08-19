--TEST--
BIND_STATIC may destroy a variable with a throwing destructor
--FILE--
<?php
class Test {
    function __destruct() {
        throw new Exception("Foo");
    }
}
try {
    $new = new Test;
    static $new;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Exception: Foo
