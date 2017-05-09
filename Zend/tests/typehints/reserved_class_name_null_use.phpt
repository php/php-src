--TEST--
'null' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as null;
--EXPECTF--
Fatal error: Cannot use foobar as null because 'null' is a special class name in %s on line %d