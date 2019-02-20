--TEST--
Bug #75921: Inconsistent error when creating stdObject from empty variable
--FILE--
<?php

$null->a = 42;
var_dump($null);
unset($null);

$null->a['hello'] = 42;
var_dump($null);
unset($null);

$null->a->b = 42;
var_dump($null);
unset($null);

$null->a['hello']->b = 42;
var_dump($null);
unset($null);

$null->a->b['hello'] = 42;
var_dump($null);
unset($null);

?>
--EXPECTF--
Warning: Attempt to assign property 'a' of non-object in %s on line %d

Notice: Undefined variable: null in %s on line %d
NULL

Warning: Attempt to modify property 'a' of non-object in %s on line %d

Notice: Undefined variable: null in %s on line %d
NULL

Notice: Undefined variable: null in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to assign property 'b' of non-object in %s on line %d

Notice: Undefined variable: null in %s on line %d
NULL

Notice: Undefined variable: null in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to assign property 'b' of non-object in %s on line %d

Notice: Undefined variable: null in %s on line %d
NULL

Notice: Undefined variable: null in %s on line %d

Notice: Trying to get property 'a' of non-object in %s on line %d

Warning: Attempt to modify property 'b' of non-object in %s on line %d

Notice: Undefined variable: null in %s on line %d
NULL
