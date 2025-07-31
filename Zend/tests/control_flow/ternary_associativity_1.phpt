--TEST--
Forbidden nested ternary, case 1
--FILE--
<?php

1 ? 2 : 3 ? 4 : 5;

?>
--EXPECTF--
Fatal error: Unparenthesized `a ? b : c ? d : e` is not supported. Use either `(a ? b : c) ? d : e` or `a ? b : (c ? d : e)` in %s on line %d
