--TEST--
show information about function
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');

var_dump(`$php -n --rf unknown`);
var_dump(`$php -n --rf echo`);
var_dump(`$php -n --rf phpinfo`);
// Regression tests for https://github.com/php/php-src/issues/21754
var_dump(`$php -n --rf ReflectionMethod::__construct`);
var_dump(`$php -n --rf ReflectionMethod::missing`);

echo "Done\n";
?>
--EXPECT--
string(45) "Exception: Function unknown() does not exist
"
string(42) "Exception: Function echo() does not exist
"
string(155) "Function [ <internal:standard> function phpinfo ] {

  - Parameters [1] {
    Parameter #0 [ <optional> int $flags = INFO_ALL ]
  }
  - Return [ true ]
}

"
string(213) "Method [ <internal:Reflection, ctor> public method __construct ] {

  - Parameters [2] {
    Parameter #0 [ <required> object|string $objectOrMethod ]
    Parameter #1 [ <optional> ?string $method = null ]
  }
}

"
string(61) "Exception: Method ReflectionMethod::missing() does not exist
"
Done
