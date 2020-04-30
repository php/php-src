--TEST--
Testing __debugInfo() magic method with bad returns object
--FILE--
<?php

class C {
  public function __debugInfo() {
    return new self();
  }
}

var_dump(new C());
--EXPECTF--
Fatal error: Method C::__debugInfo() must return an ?array in %s on line %d
