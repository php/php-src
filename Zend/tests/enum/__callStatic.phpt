--TEST--
Enum __callStatic
--FILE--
<?php

enum Foo {
    public static function __callStatic(string $name, array $args)
    {
        return [$name, $args];
    }
}

var_dump(Foo::bar('baz', 'qux'));

?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "bar"
  [1]=>
  array(2) {
    [0]=>
    string(3) "baz"
    [1]=>
    string(3) "qux"
  }
}
