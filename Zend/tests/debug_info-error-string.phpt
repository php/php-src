--TEST--
Testing __debugInfo() magic method with bad returns string
--FILE--
<?php

class C {
  public function __debugInfo() {
    return 'string';
  }
}

var_dump(new C());
--EXPECTF--
Fatal error: Method C::__debugInfo() must return an ?array in %s on line %d
