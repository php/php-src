--TEST--
The E_STRICT constant is deprecated
--FILE--
<?php

var_dump(E_ALL);
var_dump(E_STRICT);

?>
--EXPECTF--
int(30719)

Deprecated: Constant E_STRICT is deprecated in %s on line %d
int(2048)
