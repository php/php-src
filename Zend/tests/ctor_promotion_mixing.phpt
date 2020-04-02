--TEST--
Constructor promotiong mixed with other properties, parameters and code
--FILE--
<?php

class Test {
    public string $prop2;

    public function __construct(public string $prop1 = "", $param2 = "") {
        $this->prop2 = $prop1 . $param2;
    }
}

var_dump(new Test("Foo", "Bar"));
echo "\n";
echo new ReflectionClass(Test::class), "\n";

?>
--EXPECTF--
object(Test)#1 (2) {
  ["prop2"]=>
  string(6) "FooBar"
  ["prop1"]=>
  string(3) "Foo"
}

Class [ <user> class Test ] {
  @@ %s

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ public string $prop2 ]
    Property [ public string $prop1 ]
  }

  - Methods [1] {
    Method [ <user, ctor> public method __construct ] {
      @@ %s

      - Parameters [2] {
        Parameter #0 [ <optional> string $prop1 = '' ]
        Parameter #1 [ <optional> $param2 = '' ]
      }
    }
  }
}
