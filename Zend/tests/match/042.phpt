--TEST--
Match expression with undefined variable as expression
--FILE--
<?php

var_dump(match ($undefinedVariable) {
    null => 'null',
    default => 'default',
});

var_dump(match ($undefinedVariable) {
    1, 2, 3, 4, 5 => 'foo',
    default => 'bar',
});

?>
--EXPECTF--
Warning: Undefined variable $undefinedVariable in %s.php on line 3
string(4) "null"

Warning: Undefined variable $undefinedVariable in %s.php on line 8
string(3) "bar"
