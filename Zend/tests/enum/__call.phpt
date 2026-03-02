--TEST--
Enum __call
--FILE--
<?php

enum Foo {
    case Bar;

    public function __call(string $name, array $args)
    {
        return [$name, $args];
    }
}

var_dump(Foo::Bar->baz('qux', 'quux'));

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "baz"
  [1]=>
  array(2) {
    [0]=>
    string(3) "qux"
    [1]=>
    string(4) "quux"
  }
}
