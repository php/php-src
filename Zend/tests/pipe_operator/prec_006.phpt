--TEST--
Pipe precedence 006
--FILE--
<?php

$value = null;
$value
    |> fn ($x) => $x ?? throw new Exception('Value may not be null')
    |> fn ($x) => var_dump($x);

?>
--EXPECTF--
Fatal error: Arrow functions on the right hand side of |> must be parenthesized in %s on line %d
