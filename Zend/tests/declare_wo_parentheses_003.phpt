--TEST--
Testing declare w/o parentheses statement strict_types being not first statement
--FILE--
<?php

declare ticks = 1;
const foo = 'bar';
declare strict_types = 1;

?>
--EXPECTF--
Fatal error: strict_types declaration must be the very first statement in the script in %sdeclare_wo_parentheses_003.php on line 5
