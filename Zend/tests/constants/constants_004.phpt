--TEST--
Trying to redeclare constant inside namespace
--FILE--
<?php

namespace foo;

const foo = 1;
const foo = 2;

?>
--EXPECTF--
Warning: Constant foo\foo already defined, this will be an error in PHP 9 in %s on line %d
