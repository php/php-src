--TEST--
Test OCI8 LOB & Collection Class Reflection
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

reflection::export(new reflectionclass('OCI-Lob'));
reflection::export(new reflectionclass('OCI-Collection'));

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Class [ <internal:oci8> class OCI-Lob ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [22] {
    Method [ <internal%s> public method load ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method tell ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method truncate ] {

      - Parameters [1] {
        Parameter #0 [ <optional> $length ]
      }
    }

    Method [ <internal%s> public method erase ] {

      - Parameters [2] {
        Parameter #0 [ <optional> $offset ]
        Parameter #1 [ <optional> $length ]
      }
    }

    Method [ <internal%s> public method flush ] {

      - Parameters [1] {
        Parameter #0 [ <optional> $flag ]
      }
    }

    Method [ <internal%s> public method setbuffering ] {

      - Parameters [1] {
        Parameter #0 [ <required> $mode ]
      }
    }

    Method [ <internal%s> public method getbuffering ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method rewind ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method read ] {

      - Parameters [1] {
        Parameter #0 [ <required> $length ]
      }
    }

    Method [ <internal%s> public method eof ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method seek ] {

      - Parameters [2] {
        Parameter #0 [ <required> $offset ]
        Parameter #1 [ <optional> $whence ]
      }
    }

    Method [ <internal%s> public method write ] {

      - Parameters [2] {
        Parameter #0 [ <required> $string ]
        Parameter #1 [ <optional> $length ]
      }
    }

    Method [ <internal%s> public method append ] {

      - Parameters [1] {
        Parameter #0 [ <required> $lob_descriptor_from ]
      }
    }

    Method [ <internal%s> public method size ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method writetofile ] {

      - Parameters [3] {
        Parameter #0 [ <required> $filename ]
        Parameter #1 [ <optional> $start ]
        Parameter #2 [ <optional> $length ]
      }
    }

    Method [ <internal%s> public method export ] {

      - Parameters [3] {
        Parameter #0 [ <required> $filename ]
        Parameter #1 [ <optional> $start ]
        Parameter #2 [ <optional> $length ]
      }
    }

    Method [ <internal%s> public method import ] {

      - Parameters [1] {
        Parameter #0 [ <required> $filename ]
      }
    }

    Method [ <internal%s> public method writetemporary ] {

      - Parameters [2] {
        Parameter #0 [ <required> $data ]
        Parameter #1 [ <optional> $type ]
      }
    }

    Method [ <internal%s> public method close ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method save ] {

      - Parameters [2] {
        Parameter #0 [ <required> $data ]
        Parameter #1 [ <optional> $offset ]
      }
    }

    Method [ <internal%s> public method savefile ] {

      - Parameters [1] {
        Parameter #0 [ <required> $filename ]
      }
    }

    Method [ <internal%s> public method free ] {

      - Parameters [0] {
      }
    }
  }
}

Class [ <internal%s> class OCI-Collection ] {

  - Constants [0] {
  }

  - Static properties [0] {
  }

  - Static methods [0] {
  }

  - Properties [0] {
  }

  - Methods [8] {
    Method [ <internal%s> public method append ] {

      - Parameters [1] {
        Parameter #0 [ <required> $value ]
      }
    }

    Method [ <internal%s> public method getelem ] {

      - Parameters [1] {
        Parameter #0 [ <required> $index ]
      }
    }

    Method [ <internal%s> public method assignelem ] {

      - Parameters [2] {
        Parameter #0 [ <required> $index ]
        Parameter #1 [ <required> $value ]
      }
    }

    Method [ <internal%s> public method assign ] {

      - Parameters [1] {
        Parameter #0 [ <required> $collection_from ]
      }
    }

    Method [ <internal%s> public method size ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method max ] {

      - Parameters [0] {
      }
    }

    Method [ <internal%s> public method trim ] {

      - Parameters [1] {
        Parameter #0 [ <required> $number ]
      }
    }

    Method [ <internal%s> public method free ] {

      - Parameters [0] {
      }
    }
  }
}

===DONE===
