--TEST--
Partial application compile errors: named arguments must come after placeholder
--FILE--
<?php
foo(n: 5, ?);
?>
--EXPECTF--
Fatal error: Named arguments must come after all placeholders in %s on line %d

