--TEST--
Test SNMP Reflection
--SKIPIF--
<?php if (!extension_loaded('snmp')) die ("skip no snmp extension"); ?>
--FILE--
<?php

/* ALL PHP_ME user callable methods of SNMP class should appear here */

reflection::export(new reflectionmethod('snmp', '__construct'));
reflection::export(new reflectionmethod('snmp', 'close'));
reflection::export(new reflectionmethod('snmp', 'setSecurity'));
reflection::export(new reflectionmethod('snmp', 'get'));
reflection::export(new reflectionmethod('snmp', 'getnext'));
reflection::export(new reflectionmethod('snmp', 'walk'));
reflection::export(new reflectionmethod('snmp', 'set'));
reflection::export(new reflectionmethod('snmp', 'getErrno'));
reflection::export(new reflectionmethod('snmp', 'getError'));


?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Method [ <internal:snmp, ctor> public method __construct ] {

  - Parameters [5] {
    Parameter #0 [ <required> $version ]
    Parameter #1 [ <required> $host ]
    Parameter #2 [ <required> $community ]
    Parameter #3 [ <optional> $timeout ]
    Parameter #4 [ <optional> $retries ]
  }
}

Method [ <internal:snmp> public method close ] {

  - Parameters [0] {
  }
}

Method [ <internal:snmp> public method setSecurity ] {

  - Parameters [7] {
    Parameter #0 [ <required> $sec_level ]
    Parameter #1 [ <required> $auth_protocol ]
    Parameter #2 [ <required> $auth_passphrase ]
    Parameter #3 [ <required> $priv_protocol ]
    Parameter #4 [ <required> $priv_passphrase ]
    Parameter #5 [ <required> $contextName ]
    Parameter #6 [ <required> $contextEngineID ]
  }
}

Method [ <internal:snmp> public method get ] {

  - Parameters [2] {
    Parameter #0 [ <required> $object_id ]
    Parameter #1 [ <optional> $use_orignames ]
  }
}

Method [ <internal:snmp> public method getnext ] {

  - Parameters [2] {
    Parameter #0 [ <required> $object_id ]
    Parameter #1 [ <optional> $use_orignames ]
  }
}

Method [ <internal:snmp> public method walk ] {

  - Parameters [4] {
    Parameter #0 [ <required> $object_id ]
    Parameter #1 [ <required> $suffix_keys ]
    Parameter #2 [ <required> $max_repetitions ]
    Parameter #3 [ <required> $non_repeaters ]
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

===DONE===
