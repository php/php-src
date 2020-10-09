--TEST--
ZE2 class type hinting with arrays
--FILE--
<?php

class Test
{
    static function f1(array $ar)
    {
        echo __METHOD__ . "()\n";
        var_dump($ar);
    }

    static function f2(array $ar = NULL)
    {
        echo __METHOD__ . "()\n";
        var_dump($ar);
    }

    static function f3(array $ar = array())
    {
        echo __METHOD__ . "()\n";
        var_dump($ar);
    }

    static function f4(array $ar = array(25))
    {
        echo __METHOD__ . "()\n";
        var_dump($ar);
    }
}

Test::f1(array(42));
Test::f2(NULL);
Test::f2();
Test::f3();
Test::f4();
Test::f1(1);

?>
--EXPECTF--
Test::f1()
array(1) {
  [0]=>
  int(42)
}
Test::f2()
NULL
Test::f2()
NULL
Test::f3()
array(0) {
}
Test::f4()
array(1) {
  [0]=>
  int(25)
}

Fatal error: Uncaught TypeError: Test::f1(): Argument #1 ($ar) must be of type array, int given, called in %s on line %d and defined in %s:%d
Stack trace:
#0 %s(%d): Test::f1(1)
#1 {main}
  thrown in %s on line %d
