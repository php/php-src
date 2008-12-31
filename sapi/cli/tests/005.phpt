--TEST--
show information about class
--SKIPIF--
<?php 
include "skipif.inc"; 
if (!extension_loaded("reflection")) {
	die("skip");
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
string(1355) "Class [ <internal:Core> class Exception ] {

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

  - Methods [10] {
    Method [ <internal:Core> final private method __clone ] {
    }

    Method [ <internal:Core, ctor> public method __construct ] {

      - Parameters [3] {
        Parameter #0 [ <optional> $message ]
        Parameter #1 [ <optional> $code ]
        Parameter #2 [ <optional> $previous ]
      }
    }

    Method [ <internal:Core> final public method getMessage ] {
    }

    Method [ <internal:Core> final public method getCode ] {
    }

    Method [ <internal:Core> final public method getFile ] {
    }

    Method [ <internal:Core> final public method getLine ] {
    }

    Method [ <internal:Core> final public method getTrace ] {
    }

    Method [ <internal:Core> final public method getPrevious ] {
    }

    Method [ <internal:Core> final public method getTraceAsString ] {
    }

    Method [ <internal:Core> public method __toString ] {
    }
  }
}

"
Done
