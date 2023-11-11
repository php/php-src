--TEST--
Empty enum with from/tryFrom doens't segfault
--FILE--
<?php

enum A: string {}

var_dump(A::tryFrom('B'));

?>
--EXPECT--
NULL
