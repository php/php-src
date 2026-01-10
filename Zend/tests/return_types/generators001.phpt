--TEST--
Valid generator return types
--FILE--
<?php
function test1() : Generator {
    yield 1;
}

function test2() : Iterator {
    yield 2;
}

function test3() : Traversable {
    yield 3;
}

function test4() : mixed {
    yield 4;
}

function test5() : object {
    yield 5;
}

function test6() : object|callable {
    yield 6;
}

function test7() : iterable {
    yield 7;
}

var_dump(
    test1(),
    test2(),
    test3(),
    test4(),
    test5(),
    test6(),
    test7(),
);
?>
--EXPECTF--
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test1"
}
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test2"
}
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test3"
}
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test4"
}
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test5"
}
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test6"
}
object(Generator)#%d (%d) {
  ["function"]=>
  string(5) "test7"
}
