--TEST--
PFA extra named parameters are forwarded to the actual function
--FILE--
<?php
function foo(...$args) {
    var_dump($args);
}

$foo = foo(foo: "foo", ...);

$bar = $foo(bar: "bar", ...);

$baz = $bar(baz: "baz", ...);

$baz();

$foo = foo(...);

$bar = $foo(bar: "bar", ...);

$baz = $bar(baz: "baz", ...);

$baz();

$foo = foo(foo: "foo", ...);

$foo(bar: "bar");
?>
--EXPECT--
array(3) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
  ["baz"]=>
  string(3) "baz"
}
array(2) {
  ["bar"]=>
  string(3) "bar"
  ["baz"]=>
  string(3) "baz"
}
array(2) {
  ["foo"]=>
  string(3) "foo"
  ["bar"]=>
  string(3) "bar"
}
