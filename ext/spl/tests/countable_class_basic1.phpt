--TEST--
SPL: Test shape of interface Countable.
--SKIPIF--
<?php 
// Skip the test case if Standard PHP Library(spl) is not installed
  if( !extension_loaded('spl'))
  {
     die('skip spl is not installed');
  }
?>
--FILE--
<?php
ReflectionClass::export('Countable');
?>
--EXPECTF--
Interface [ <internal%s> interface Countable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [1] {
    Method [ <internal%s> abstract public method count ] {

      - Parameters [0] {
      }
    }
  }
}
