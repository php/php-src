--TEST--
GH-22681: null bytes in backed value truncate ReflectionEnum::__toString()
--FILE--
<?php

enum Demo: string {
	case DEMO = "F\0oo";
}

$r = new ReflectionEnum(Demo::class);
echo $r;
var_dump( new ReflectionEnumBackedCase(Demo::class, 'DEMO')->getBackingValue() );
?>
--EXPECTF--
Enum [ <user> enum Demo: string implements UnitEnum, BackedEnum ] {
  @@ %s %d-%d

  - Enum cases [1] {
    Case DEMO = F
  }

  - Constants [0] {
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
    Property [ public protected(set) readonly string $value ]
  }

  - Methods [0] {
  }
}
string(4) "F%0oo"
