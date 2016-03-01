--TEST--
Bug #71678 class() extends self with final method
--FILE--
<?php
class Test {

  static function newSelf() {
    return new class() extends self {
      function thatsIt() { }
    };
  }

  final function thatsIt() {
    // Cannot be overridden!
  }
}

Test::newSelf();
--EXPECTF--
Fatal error: Cannot override final method Test::thatsIt() in %s on line %d