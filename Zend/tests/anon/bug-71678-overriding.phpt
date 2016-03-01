--TEST--
Bug #71678 class() extends self overriding with incompatible signature
--FILE--
<?php
class Test {

  static function newSelf() {
    return new class() extends self {
      function thatsIt($pre) { }
    };
  }

  function thatsIt() {
    // Base
  }
}

Test::newSelf();
--EXPECTF--
Warning: Declaration of %s@anonymous::thatsIt($pre) should be compatible with Test::thatsIt() in %s on line %d