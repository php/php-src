--TEST--
Void cannot be used as a class, trait or interface name - use
--FILE--
<?php

use foobar as void;
--EXPECTF--
Fatal error: "void" cannot be used as a class name in %s on line %d
