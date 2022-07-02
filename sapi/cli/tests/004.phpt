--TEST--
show information about function
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n --rf unknown`);
var_dump(`$php -n --rf echo`);
var_dump(`$php -n --rf phpinfo`);

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
Done
