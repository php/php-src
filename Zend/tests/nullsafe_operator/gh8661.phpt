--TEST--
GH-8661: Nullsafe in coalesce triggers undefined variable error
--FILE--
<?php

var_dump($a?->foo ?? null);

?>
--EXPECT--
NULL
