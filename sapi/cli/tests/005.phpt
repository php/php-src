--TEST--
show information about class
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`"$php" -n --rc unknown`);
var_dump(`"$php" -n --rc stdclass`);
var_dump(`"$php" -n --rc exception`);

echo "Done\n";
?>
--EXPECT--
string(40) "Exception: Class unknown does not exist
"
string(183) "Class [ <internal:Core> class stdClass ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [0] {
  }
}

"
string(1969) "Class [ <internal:Core> class Exception implements Throwable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [7] {
    Property [ <default> protected $message ]
    Property [ <default> private $string ]
    Property [ <default> protected $code ]
    Property [ <default> protected $file ]
    Property [ <default> protected $line ]
    Property [ <default> private $trace ]
    Property [ <default> private $previous ]
  }

  - Methods [11] {
    Method [ <internal:Core> final private method __clone ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, ctor> public method __construct ] {

      - Parameters [3] {
        Parameter #0 [ <optional> string $message ]
        Parameter #1 [ <optional> int $code ]
        Parameter #2 [ <optional> ?Throwable $previous ]
      }
    }

    Method [ <internal:Core> public method __wakeup ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getMessage ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getCode ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getFile ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getLine ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getTrace ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getPrevious ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getTraceAsString ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> public method __toString ] {

      - Parameters [0] {
      }
    }
  }
}

"
Done
