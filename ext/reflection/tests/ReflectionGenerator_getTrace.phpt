--TEST--
ReflectionGenerator::getTrace() over multiple Generators
--FILE--
<?php

function foo() {
    yield 1;
    yield 2;
}

function bar()
{
    yield from foo();
}

function baz()
{
    yield from bar();
}

$gen = baz();
$gen->valid();

var_dump((new ReflectionGenerator($gen))->getTrace());

?>
--EXPECTF--
array(3) {
  [0]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(3) "foo"
    ["args"]=>
    array(0) {
    }
  }
  [1]=>
  array(4) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(3) "bar"
    ["args"]=>
    array(0) {
    }
  }
  [2]=>
  array(2) {
    ["function"]=>
    string(3) "baz"
    ["args"]=>
    array(0) {
    }
  }
}
