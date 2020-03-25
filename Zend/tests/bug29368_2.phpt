--TEST--
Bug #29368.2 (The destructor is called when an exception is thrown from the constructor).
--FILE--
<?php
class Bomb {
    function foo() {
    }
    function __destruct() {
        throw new Exception("bomb!");
    }
}
try {
    $x = new ReflectionMethod(new Bomb(), "foo");
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}
echo "ok\n";
?>
--EXPECT--
bomb!
ok
