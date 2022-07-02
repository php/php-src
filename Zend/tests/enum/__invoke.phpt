--TEST--
Enum __invoke
--FILE--
<?php

enum Foo {
    case Bar;

    public function __invoke(...$args)
    {
        return $args;
    }
}

var_dump((Foo::Bar)('baz', 'qux'));

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "baz"
  [1]=>
  string(3) "qux"
}
