--TEST--
'false' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as false;
--EXPECTF--
Fatal error: Cannot use foobar as false because 'false' is a special class name in %s on line %d