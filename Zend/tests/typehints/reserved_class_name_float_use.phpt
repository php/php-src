--TEST--
'float' cannot be used as a class, interface or trait name - use
--FILE--
<?php

use foobar as float;
--EXPECTF--
Fatal error: Cannot use foobar as float because 'float' is a special class name in %s on line %d