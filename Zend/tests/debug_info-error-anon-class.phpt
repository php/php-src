--TEST--
Testing __debugInfo() magic method with invalid returns inside anon-class
--FILE--
<?php

class C {
  public function __debugInfo() {
    return new class {
      public function __debugInfo() {
        return 1;
      }
    };
  }
}

var_dump(new C());
--EXPECTF--
Fatal error: Method C::__debugInfo() must return an ?array in %s on line %d
