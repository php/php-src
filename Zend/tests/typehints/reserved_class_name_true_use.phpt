--TEST--
'true' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as true;
--EXPECTF--
Fatal error: Cannot use foobar as true because 'true' is a special class name in %s on line %d