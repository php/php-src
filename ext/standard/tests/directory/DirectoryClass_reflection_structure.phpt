--TEST--
Directory class behaviour.
--FILE--
<?php
/*
 * Description: Directory class with properties, handle and class and methods read, rewind and close
 * Class is defined in ext/standard/dir.c
 */

echo "Structure of Directory class:\n";
$rc = new ReflectionClass("Directory");
echo $rc;

?>
--EXPECT--
Structure of Directory class:
Class [ <internal:standard> final class Directory ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [2] {
    Property [ public protected(set) readonly string $path ]
    Property [ public protected(set) readonly mixed $handle ]
  }

  - Methods [3] {
    Method [ <internal:standard> public method close ] {

      - Parameters [0] {
      }
      - Return [ void ]
    }

    Method [ <internal:standard> public method rewind ] {

      - Parameters [0] {
      }
      - Return [ void ]
    }

    Method [ <internal:standard> public method read ] {

      - Parameters [0] {
      }
      - Return [ string|false ]
    }
  }
}
