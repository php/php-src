--TEST--
Bug #69751: Change Error message of sprintf/printf for missing/typo position specifier.
--FILE--
<?php

sprintf('%$s, %2$s %1$s', "a", "b");
sprintf('%500$s, %2$s %1$s', "a", "b");
sprintf('%2147483648$s, %2$s %1$s', "a", "b");

?>
--EXPECTF--
Warning: sprintf(): Argument number specifier must be greater than zero and less than %d in %s on line %d

Warning: sprintf(): Too few arguments in %s on line %d

Warning: sprintf(): Argument number specifier must be greater than zero and less than %d in %s on line %d
