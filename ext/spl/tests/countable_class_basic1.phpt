--TEST--
SPL: Test shape of interface Countable.
--SKIPIF--
<?php if (!extension_loaded('spl') || !extension_loaded('reflection')) print 'skip'; ?>
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
    }
  }
}
