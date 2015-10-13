--TEST--
'string' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as string;
--EXPECTF--
Fatal error: Cannot use foobar as string because 'string' is a special class name in %s on line %d