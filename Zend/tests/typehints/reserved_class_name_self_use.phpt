--TEST--
'self' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as self;
--EXPECTF--
Fatal error: Cannot use foobar as self because 'self' is a special class name in %s on line %d