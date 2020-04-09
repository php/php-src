--TEST--
Match expression with undefined variable as expression
--FILE--
<?php

var_dump(match ($undefinedVariable) {
    null => null,
    default => 'default',
});

?>
--EXPECTF--
Warning: Undefined variable $undefinedVariable in %s.php on line 3
NULL
