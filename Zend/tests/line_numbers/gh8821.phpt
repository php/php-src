--TEST--
GH-8821: Fix reported line number of constant expression
--FILE--
<?php

enum Alpha {
    case Foo;
}

class Bravo {
    public const C = [Alpha::Foo => 3];
}

new Bravo();

?>
--EXPECTF--
Fatal error: Uncaught TypeError: Cannot access offset of type Alpha on array in %sgh8821.php:8
Stack trace:
#0 %sgh8821.php(11): [constant expression]()
#1 {main}
  thrown in %sgh8821.php on line 8
