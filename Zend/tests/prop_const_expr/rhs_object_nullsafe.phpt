--TEST--
Nullsafe property constant expression rhs wrong type
--FILE--
<?php

class A {}
class B {}

const A_prop = (new A)?->{new B};

var_dump(A_prop);

?>
--EXPECTF--
Fatal error: Uncaught Error: Object of class B could not be converted to string in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
