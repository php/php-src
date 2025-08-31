--TEST--
Pipe precedence 001
--FILE--
<?php

42
    |> fn($x) => $x < 42
    |> fn($x) => var_dump($x);

?>
--EXPECTF--
Fatal error: Arrow functions on the right hand side of |> must be parenthesized in %s on line %d
