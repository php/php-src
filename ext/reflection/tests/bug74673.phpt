--TEST--
Bug #74673 (Segfault when cast Reflection object to string with undefined constant)
--FILE--
<?php

class A
{
    public function method($test = PHP_SELF + 1)
    {
    }
}

$class = new ReflectionClass('A');

echo $class;
?>
--EXPECTF--
Class [ <user> class A ] {
  @@ %s

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [1] {
    Method [ <user> public method method ] {
      @@ %s

      - Parameters [1] {
        Parameter #0 [ <optional> $test = PHP_SELF + 1 ]
      }
    }
  }
}
