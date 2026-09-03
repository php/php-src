--TEST--
GH-19304: Incorrect anonymous class type name assertion
--FILE--
<?php

$foo = new class {
    public self $v;
};

try {
    $foo->v = 0;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
TypeError: Cannot assign int to property class@anonymous%0%s of type self
