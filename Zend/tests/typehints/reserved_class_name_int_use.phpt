--TEST--
'int' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as int;
--EXPECTF--
Fatal error: Cannot use foobar as int because 'int' is a special class name in %s on line %d