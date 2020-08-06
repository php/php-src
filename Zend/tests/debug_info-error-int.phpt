--TEST--
Testing __debugInfo() magic method with bad returns int
--FILE--
<?php

class C {
  public function __debugInfo() {
    return 2;
  }
}

var_dump(new C());
--EXPECTF--
Fatal error: Method C::__debugInfo() must return an ?array in %s on line %d
