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
string(42) "Exception: Class "unknown" does not exist
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
string(2194) "Class [ <internal:Core> class Exception implements Stringable, Throwable ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [7] {
    Property [ protected $message = '' ]
    Property [ private $string = '' ]
    Property [ protected $code = 0 ]
    Property [ protected $file = NULL ]
    Property [ protected $line = NULL ]
    Property [ private array $trace = Array ]
    Property [ private ?Throwable $previous = NULL ]
  }

  - Methods [11] {
    Method [ <internal:Core> final private method __clone ] {

      - Parameters [0] {
      }
      - Return [ void ]
    }

    Method [ <internal:Core, ctor> public method __construct ] {

      - Parameters [3] {
        Parameter #0 [ <optional> string $message = "" ]
        Parameter #1 [ <optional> int $code = 0 ]
        Parameter #2 [ <optional> ?Throwable $previous = null ]
      }
    }

    Method [ <internal:Core> public method __wakeup ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getMessage ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }

    Method [ <internal:Core, prototype Throwable> final public method getCode ] {

      - Parameters [0] {
      }
    }

    Method [ <internal:Core, prototype Throwable> final public method getFile ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }

    Method [ <internal:Core, prototype Throwable> final public method getLine ] {

      - Parameters [0] {
      }
      - Return [ int ]
    }

    Method [ <internal:Core, prototype Throwable> final public method getTrace ] {

      - Parameters [0] {
      }
      - Return [ array ]
    }

    Method [ <internal:Core, prototype Throwable> final public method getPrevious ] {

      - Parameters [0] {
      }
      - Return [ ?Throwable ]
    }

    Method [ <internal:Core, prototype Throwable> final public method getTraceAsString ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }

    Method [ <internal:Core, prototype Stringable> public method __toString ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}

"
Done
