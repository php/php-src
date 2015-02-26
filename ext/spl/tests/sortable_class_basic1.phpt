--TEST--
SPL: Test shape of interface Sortable.
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
ReflectionClass::export('Sortable');
?>
--EXPECTF--
Interface [ <internal%s> interface Sortable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [2] {
    Method [ <internal%s> abstract public method sort ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> abstract public method usort ] {

      - Parameters [1] {
        Parameter #0 [ <required> $cmp_function ]
      }
    }
  }
}
