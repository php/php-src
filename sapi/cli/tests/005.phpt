--TEST--
show information about class
--SKIPIF--
<?php
include "skipif.inc";
if (!extension_loaded("reflection")) {
    die("skip reflection extension required");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`"$php" -n --rc unknown`);
var_dump(`"$php" -n --rc stdclass`);
var_dump(`"$php" -n --rc exception`);

echo "Done\n";
?>
--EXPECTF--
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
string(1607) "Class [ <internal:Core> class Exception implements Throwable ] {

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
    }

    Method [ <internal:Core, ctor> public method __construct ] {

      - Parameters [3] {
        Parameter #0 [ <optional> $message ]
        Parameter #1 [ <optional> $code ]
        Parameter #2 [ <optional> $previous ]
      }
    }

    Method [ <internal:Core> public method __wakeup ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getMessage ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getCode ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getFile ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getLine ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getTrace ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getPrevious ] {
    }

    Method [ <internal:Core, prototype Throwable> final public method getTraceAsString ] {
    }

    Method [ <internal:Core, prototype Throwable> public method __toString ] {
    }
  }
}

"
Done
