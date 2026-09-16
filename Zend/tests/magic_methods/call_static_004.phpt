--TEST--
Invalid method name in dynamic static call
--FILE--
<?php

class foo {
    static function __callstatic($a, $b) {
        var_dump($a);
    }
}

foo::AaA();

$a = 1;
foo::$a();

?>
--EXPECTF--
string(3) "AaA"

Fatal error: Uncaught Error: Method name must be a string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
