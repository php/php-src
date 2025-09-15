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
--EXPECT--
Cannot assign int to property class@anonymous::$v of type self
