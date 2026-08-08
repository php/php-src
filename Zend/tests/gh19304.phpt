--TEST--
GH-19304: Incorrect anonymous class type name assertion
--FILE--
<?php

$foo = new class {
    public self $v;
};

try {
    $foo->v = 0;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Cannot assign int to property class@anonymous%0%s:3$0::$v of type self
