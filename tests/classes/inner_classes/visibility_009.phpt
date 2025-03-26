--TEST--
deeply nested property visibility
--FILE--
<?php

class Outer {
    private int $i;
    private static int $j;
    class Middle {
        private int $i;
        private static int $j;
        class Inner {
            public static function test() {
                var_dump(Outer::$j = 5);
                var_dump(Middle::$j = 42);
                $foo = new Outer();
                $foo->i = 42;
                var_dump($foo);
                $foo = new Middle();
                $foo->i = 42;
                var_dump($foo);
            }
        }
    }
}
Outer\Middle\Inner::test();
?>
--EXPECT--
int(5)
int(42)
object(Outer)#1 (1) {
  ["i":"Outer":private]=>
  int(42)
}
object(Outer\Middle)#2 (1) {
  ["i":"Outer\Middle":private]=>
  int(42)
}
