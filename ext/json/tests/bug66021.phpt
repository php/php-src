--TEST--
Bug #66021 (Blank line inside empty array/object when JSON_PRETTY_PRINT is set)
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
