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
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Foo
