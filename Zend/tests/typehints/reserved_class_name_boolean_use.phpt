--TEST--
'boolean' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as boolean;
--EXPECTF--
Fatal error: Cannot use foobar as boolean because 'boolean' is a special class name in %s on line %d