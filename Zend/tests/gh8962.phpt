--TEST--
GH-8962: Display function name when var_dumping fake closure
--FILE--
<?php

namespace Foo {
    class Bar {
        public function baz() {}
        public static function qux() {}
    }

    function quux() {}

    var_dump(\Closure::fromCallable([new Bar(), 'baz']));
    var_dump(\Closure::fromCallable([Bar::class, 'qux']));
    var_dump(\Closure::fromCallable('Foo\Bar::qux'));
    var_dump(quux(...));
}

?>
--EXPECT--
object(Closure)#2 (2) {
  ["function"]=>
  string(12) "Foo\Bar::baz"
  ["this"]=>
  object(Foo\Bar)#1 (0) {
  }
}
object(Closure)#2 (1) {
  ["function"]=>
  string(12) "Foo\Bar::qux"
}
object(Closure)#2 (1) {
  ["function"]=>
  string(12) "Foo\Bar::qux"
}
object(Closure)#2 (1) {
  ["function"]=>
  string(8) "Foo\quux"
}
