--TEST--
Test SNMP Reflection
--SKIPIF--
<?php if (!extension_loaded('snmp')) die ("skip no snmp extension"); ?>
--FILE--
<?php

/* ALL PHP_ME user callable methods of SNMP class should appear here */

echo new reflectionmethod('snmp', '__construct');
echo new reflectionmethod('snmp', 'close');
echo new reflectionmethod('snmp', 'setSecurity');
echo new reflectionmethod('snmp', 'get');
echo new reflectionmethod('snmp', 'getnext');
echo new reflectionmethod('snmp', 'walk');
echo new reflectionmethod('snmp', 'set');
echo new reflectionmethod('snmp', 'getErrno');
echo new reflectionmethod('snmp', 'getError');


?>
--EXPECT--
Method [ <internal:snmp, ctor> public method __construct ] {

  - Parameters [5] {
    Parameter #0 [ <required> int $version ]
    Parameter #1 [ <required> string $host ]
    Parameter #2 [ <required> string $community ]
    Parameter #3 [ <optional> int $timeout ]
    Parameter #4 [ <optional> int $retries ]
  }
}
Method [ <internal:snmp> public method close ] {

  - Parameters [0] {
  }
}
Method [ <internal:snmp> public method setSecurity ] {

  - Parameters [7] {
    Parameter #0 [ <required> string $sec_level ]
    Parameter #1 [ <optional> string $auth_protocol ]
    Parameter #2 [ <optional> string $auth_passphrase ]
    Parameter #3 [ <optional> string $priv_protocol ]
    Parameter #4 [ <optional> string $priv_passphrase ]
    Parameter #5 [ <optional> string $contextName ]
    Parameter #6 [ <optional> string $contextEngineID ]
  }
}
Method [ <internal:snmp> public method get ] {

  - Parameters [2] {
    Parameter #0 [ <required> $object_id ]
    Parameter #1 [ <optional> bool $use_orignames ]
  }
}
Method [ <internal:snmp> public method getnext ] {

  - Parameters [1] {
    Parameter #0 [ <required> $object_id ]
  }
}
Method [ <internal:snmp> public method walk ] {

  - Parameters [4] {
    Parameter #0 [ <required> $object_id ]
    Parameter #1 [ <optional> bool $suffix_keys ]
    Parameter #2 [ <optional> int $max_repetitions ]
    Parameter #3 [ <optional> int $non_repeaters ]
  }
}
Method [ <internal:snmp> public method set ] {

  - Parameters [3] {
    Parameter #0 [ <required> $object_id ]
    Parameter #1 [ <required> $type ]
    Parameter #2 [ <required> $value ]
  }
}
Method [ <internal:snmp> public method getErrno ] {

  - Parameters [0] {
  }
}
Method [ <internal:snmp> public method getError ] {

  - Parameters [0] {
  }
}
