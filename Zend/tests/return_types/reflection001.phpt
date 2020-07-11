--TEST--
Return type and Reflection::export()
--FILE--
<?php

class A {
    function foo(array $a): array {
        return $a;
    }
}

echo new ReflectionClass("A");
?>
--EXPECTF--
Class [ <user> class A ] {
  @@ %sreflection001.php 3-7

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [1] {
    Method [ <user> public method foo ] {
      @@ %sreflection001.php 4 - 6

      - Parameters [1] {
        Parameter #0 [ <required> array $a ]
      }
      - Return [ array ]
    }
  }
}
