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
try {
    array_map($cb, null);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    array_map($cb, null, null);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
array_filter([], $cb);
array_reduce([], $cb);

$array = [];
array_walk($array, $cb);
array_walk_recursive($array, $cb);
usort($array, $cb);

try {
    preg_replace_callback('/./', $cb, new stdClass);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECT--
array_map(): Argument #2 ($array) must be of type array, null given
array_map(): Argument #2 ($array) must be of type array, null given
preg_replace_callback(): Argument #3 ($subject) must be of type array|string, stdClass given
===DONE===
