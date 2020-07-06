--TEST--
Cannot use special class name as namespace
--FILE--
<?php

namespace self;

?>
--EXPECTF--
Fatal error: "self" cannot be used as namespace name in %s on line %d
