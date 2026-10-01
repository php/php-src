--TEST--
RHS gets evaluated before throwing error when accessing properties on non-enums in constant expressions
--FILE--
<?php

class A {
    public $prop = 42;
}

class Printer {
    public function __construct() {
        echo "Printer\n";
        return 'printer';
    }
}

const A_prop = (new A)->{new Printer ? 'printer' : null};

?>
--EXPECTF--
Deprecated: Returning a value from a constructor is deprecated in %s on line %d
Printer

Fatal error: Uncaught Error: Fetching properties on non-enums in constant expressions is not allowed in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
