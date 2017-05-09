--TEST--
'bool' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as bool;
--EXPECTF--
Fatal error: Cannot use foobar as bool because 'bool' is a special class name in %s on line %d