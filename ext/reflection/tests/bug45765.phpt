--TEST--
Fixed bug #45765 (ReflectionObject with default parameters of self::xxx cause an error)
--FILE--
<?php

class foo2 {
    const BAR = 'foobar';
}

class foo extends foo2 {
    const BAR = "foo's bar";

    function test($a = self::BAR) {
    }

    function test2($a = parent::BAR) {
    }

    function test3($a = foo::BAR) {
    }

    function test4($a = foo2::BAR) {
    }
}

echo new ReflectionObject(new foo);

?>
--EXPECTF--
Object of class [ <user> class foo extends foo2 ] {
  @@ %s 7-21

  - Constants [1] {
    Constant [ public string BAR ] { foo's bar }
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Dynamic properties [0] {
  }

  - Methods [4] {
    Method [ <user> public method test ] {
      @@ %s 10 - 11

      - Parameters [1] {
        Parameter #0 [ <optional> $a = 'foo's bar' ]
      }
    }

    Method [ <user> public method test2 ] {
      @@ %s 13 - 14

      - Parameters [1] {
        Parameter #0 [ <optional> $a = 'foobar' ]
      }
    }

    Method [ <user> public method test3 ] {
      @@ %s 16 - 17

      - Parameters [1] {
        Parameter #0 [ <optional> $a = 'foo's bar' ]
      }
    }

    Method [ <user> public method test4 ] {
      @@ %s 19 - 20

      - Parameters [1] {
        Parameter #0 [ <optional> $a = 'foobar' ]
      }
    }
  }
}
