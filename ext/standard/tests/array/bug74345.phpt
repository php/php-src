--TEST--
Bug #74345: Call trampoline leaked if callback not invoked
--FILE--
<?php

class Test {
    public function __call($name, $args) {
        echo "__call()\n";
    }
}

$name = "foo" . ($x = "bar");
$cb = [new Test, $name];
array_map($cb, []);
array_map($cb, [], []);
array_filter([], $cb);
array_reduce([], $cb);

$array = [];
array_walk($array, $cb);
array_walk_recursive($array, $cb);
usort($array, $cb);

?>
===DONE===
--EXPECT--
===DONE===
