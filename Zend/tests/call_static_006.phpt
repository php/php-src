--TEST--
Testing __callStatic
--FILE--
<?php

class foo {
    static function __callstatic($a, $b) {
        var_dump($a);
    }
}

foo::__construct();

?>
--EXPECTF--
Fatal error: Uncaught Error: Cannot call constructor in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
