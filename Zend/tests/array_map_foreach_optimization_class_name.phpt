--TEST--
array_map(): foreach optimization resolves the class name of a static FCC
--FILE--
<?php

namespace Foo {
    class Bar {
        public static function double(int $x): int {
            return $x * 2;
        }
    }

    class Parent_ {
        public static function triple(int $x): int {
            return $x * 3;
        }
    }

    class Child extends Parent_ {
        public static function scoped(): void {
            var_dump(\array_map(self::triple(...), [1]));
            var_dump(\array_map(static::triple(...), [1]));
            var_dump(\array_map(parent::triple(...), [1]));
        }
    }

    var_dump(\array_map(Bar::double(...), [1]));
    Child::scoped();
}

namespace {
    use Foo\Bar;
    use Foo\Bar as Baz;

    var_dump(array_map(Bar::double(...), [1]));
    var_dump(array_map(Baz::double(...), [1]));
    var_dump(array_map(\Foo\Bar::double(...), [1]));
    var_dump(array_map(namespace\Foo\Bar::double(...), [1]));
}

?>
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [0]=>
  int(3)
}
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(2)
}
array(1) {
  [0]=>
  int(2)
}
