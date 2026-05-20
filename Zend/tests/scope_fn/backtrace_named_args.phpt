--TEST--
debug_backtrace() includes extra named args for scope functions
--FILE--
<?php
function test() {
    $fn = fn(int $a, ...$args) {
        $trace = debug_backtrace(0);
        var_dump($trace[0]['args']);
    };
    $fn(1, 2, name1: "hello", name2: "world");
}
test();

function only_named() {
    $fn = fn(...$args) {
        $trace = debug_backtrace(0);
        var_dump($trace[0]['args']);
    };
    $fn(only: "named");
}
only_named();

function no_extras() {
    $fn = fn(int $a, int $b) {
        $trace = debug_backtrace(0);
        var_dump($trace[0]['args']);
    };
    $fn(1, 2);
}
no_extras();
?>
--EXPECT--
array(4) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  ["name1"]=>
  string(5) "hello"
  ["name2"]=>
  string(5) "world"
}
array(1) {
  ["only"]=>
  string(5) "named"
}
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
