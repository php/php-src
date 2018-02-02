--TEST--
Bug #66021 (Blank line inside empty array/object when JSON_PRETTY_PRINT is set)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

class Foo {
	private $bar = 'baz';
}

echo json_encode(array(array(), (object) array(), new Foo), JSON_PRETTY_PRINT);

?>
--EXPECT--
[
    [],
    {},
    {}
]
