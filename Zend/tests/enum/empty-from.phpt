--TEST--
Empty enum with from/tryFrom doesn't segfault
--FILE--
<?php

enum A: string {}

var_dump(A::tryFrom('B'));

?>
--EXPECT--
NULL
