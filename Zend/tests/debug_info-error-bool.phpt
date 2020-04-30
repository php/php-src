--TEST--
Testing __debugInfo() magic method with bad returns bool
--FILE--
<?php

class C {
  public function __debugInfo() {
    return true;
  }
}

var_dump(new C());
--EXPECTF--
Fatal error: Method C::__debugInfo() must return an ?array in %s on line %d
