--TEST--
'double' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as double;
--EXPECTF--
Fatal error: Cannot use foobar as double because 'double' is a special class name in %s on line %d