--TEST--
Bug #71678 class() extends self with final
--FILE--
<?php
final class Test {

  static function newSelf() {
    return new class() extends self { };
  }
}

Test::newSelf();
--EXPECTF--
Fatal error: Class %s@anonymous may not inherit from final class (Test) in %s on line %d