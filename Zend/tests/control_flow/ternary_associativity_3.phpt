--TEST--
Forbidden nested ternary, case 3
--FILE--
<?php

1 ? 2 : 3 ?: 4;

?>
--EXPECTF--
Fatal error: Unparenthesized `a ? b : c ?: d` is not supported. Use either `(a ? b : c) ?: d` or `a ? b : (c ?: d)` in %s on line %d
