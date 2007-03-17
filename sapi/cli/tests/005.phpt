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

var_dump(`$php -n --rc unknown`);
var_dump(`$php -n --rc stdclass`);
var_dump(`$php -n --rc exception`);

echo "Done\n";
?>
--EXPECTF--	
string(40) "Exception: Class unknown does not exist
"
string(178) "Class [ <internal> class stdClass ] {

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
string(1141) "Class [ <internal> class Exception ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [6] {
    Property [ <default> protected $message ]
    Property [ <default> private $string ]
    Property [ <default> protected $code ]
    Property [ <default> protected $file ]
    Property [ <default> protected $line ]
    Property [ <default> private $trace ]
  }

  - Methods [9] {
    Method [ <internal> final private method __clone ] {
    }

    Method [ <internal, ctor> public method __construct ] {

      - Parameters [2] {
        Parameter #0 [ <optional> $message ]
        Parameter #1 [ <optional> $code ]
      }
    }

    Method [ <internal> final public method getMessage ] {
    }

    Method [ <internal> final public method getCode ] {
    }

    Method [ <internal> final public method getFile ] {
    }

    Method [ <internal> final public method getLine ] {
    }

    Method [ <internal> final public method getTrace ] {
    }

    Method [ <internal> final public method getTraceAsString ] {
    }

    Method [ <internal> public method __toString ] {
    }
  }
}

"
Done
