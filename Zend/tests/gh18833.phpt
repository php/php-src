--TEST--
GH-18833 (Use after free with weakmaps dependent on destruction order)
--FILE--
<?php

class a {
    public static WeakMap $map;
    public static Generator $storage;
}

a::$map = new WeakMap;

$closure = function () {
    $obj = new a;
    a::$map[$obj] = true;
    yield $obj;
};
a::$storage = $closure();
a::$storage->current();

echo "ok\n";
?>
--EXPECT--
ok
