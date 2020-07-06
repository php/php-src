--TEST--
ZE2 object cloning, 5
--FILE--
<?php
abstract class base {
  public $a = 'base';

  // disallow cloning once forever
  final protected function __clone() {}
}

class test extends base {
  // reenabling should fail
  public function __clone() {}
}

?>
--EXPECTF--
Fatal error: Method test::__clone() cannot override final method base::__clone() in %s on line %d
