--TEST--
GH-22681: null bytes in doc comment truncate ReflectionEnum::__toString()
--FILE--
<?php

eval(<<<END
enum Demo {
    /** F\0oo */
    case C;
}
END
);

$r = new ReflectionEnum(Demo::class);
echo $r;
var_dump( new ReflectionEnumUnitCase(Demo::class, 'C')->getDocComment() );
?>
--EXPECTF--
Class [ <user> final class Demo implements UnitEnum ] {
  @@ %s(%d) : eval()'d code %d-%d

  - Constants [1] {
    /** F%0oo */
    Constant [ public Demo C ] { Object }
  }

  - Static properties [0] {
  }

  - Static methods [1] {
    Method [ <internal, prototype UnitEnum> static public method cases ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }
  }

  - Properties [1] {
    Property [ public protected(set) readonly string $name ]
  }

  - Methods [0] {
  }
}
string(11) "/** F%0oo */"
