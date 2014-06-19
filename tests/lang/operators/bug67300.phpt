--TEST--
Test casting (object) array() for bug #67300
--DESCRIPTION--
This tests if casting an array with an empty property key into an object returns a fatal error.
--FILE--
<?php
$object = (object) array(
    "foo" => 42,
    "boo" => 37,
    "bar" => 19,
);
print_r($object);

var_dump((object) array(
    "foo2" => "test foo",
    2 => 37,
    "bar" => 19,
));

(object) array(
    "foo" => 42,
    ""    => 37,
    "bar" => 19,
);
?>
--EXPECTF--
stdClass Object
(
    [foo] => 42
    [boo] => 37
    [bar] => 19
)
object(stdClass)#2 (3) {
  ["foo2"]=>
  string(8) "test foo"
  [2]=>
  int(37)
  ["bar"]=>
  int(19)
}

Fatal error: Cannot convert an array with empty keys into an object in %s on line %d
