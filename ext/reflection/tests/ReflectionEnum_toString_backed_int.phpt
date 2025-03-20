--TEST--
ReflectionEnum::__toString() (larger case, int-backed)
--FILE--
<?php

interface MyStringable {
    public function toString(): string;
}

enum MyBool: int implements MyStringable {
    case MyFalse = 0;
    case MyTrue = 1;
    
    public const MyBool OtherTrue = MyBool::MyTrue;

    public function toString(): string {
        return $this->name . " = " . $this->value;
    }
}

$r = new ReflectionClass( MyBool::class );
echo $r;
echo "\n";
$r = new ReflectionEnum( MyBool::class );
echo $r;

var_export( MyBool::cases() );

?>
--EXPECTF--
Enum [ <user> enum MyBool: int implements MyStringable, UnitEnum, BackedEnum ] {
  @@ %sReflectionEnum_toString_backed_int.php 7-16

  - Enum cases [2] {
    Case MyFalse = 0
    Case MyTrue = 1
  }

  - Constants [1] {
    Constant [ public MyBool OtherTrue ] { Object }
  }

  - Static properties [0] {
  }

  - Static methods [3] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }

    Method [ <internal, prototype BackedEnum> static public method from ] {

      - Parameters [1] {
        Parameter #0 [ <required> string|int $value ]
      }
      - Return [ static ]
    }

    Method [ <internal, prototype BackedEnum> static public method tryFrom ] {

      - Parameters [1] {
        Parameter #0 [ <required> string|int $value ]
      }
      - Return [ ?static ]
    }
  }

  - Properties [2] {
    Property [ public protected(set) readonly string $name ]
    Property [ public protected(set) readonly int $value ]
  }

  - Methods [1] {
    Method [ <user, prototype MyStringable> public method toString ] {
      @@ %sReflectionEnum_toString_backed_int.php 13 - 15

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}

Enum [ <user> enum MyBool: int implements MyStringable, UnitEnum, BackedEnum ] {
  @@ %sReflectionEnum_toString_backed_int.php 7-16

  - Enum cases [2] {
    Case MyFalse = 0
    Case MyTrue = 1
  }

  - Constants [1] {
    Constant [ public MyBool OtherTrue ] { Object }
  }

  - Static properties [0] {
  }

  - Static methods [3] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }

    Method [ <internal, prototype BackedEnum> static public method from ] {

      - Parameters [1] {
        Parameter #0 [ <required> string|int $value ]
      }
      - Return [ static ]
    }

    Method [ <internal, prototype BackedEnum> static public method tryFrom ] {

      - Parameters [1] {
        Parameter #0 [ <required> string|int $value ]
      }
      - Return [ ?static ]
    }
  }

  - Properties [2] {
    Property [ public protected(set) readonly string $name ]
    Property [ public protected(set) readonly int $value ]
  }

  - Methods [1] {
    Method [ <user, prototype MyStringable> public method toString ] {
      @@ %sReflectionEnum_toString_backed_int.php 13 - 15

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}
array (
  0 => 
  \MyBool::MyFalse,
  1 => 
  \MyBool::MyTrue,
)
