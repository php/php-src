--TEST--
Pipe precedence 003
--FILE--
<?php

null
    |> fn() => print (new Exception)->getTraceAsString() . "\n\n"
    |> fn() => print (new Exception)->getTraceAsString() . "\n\n";

?>
--EXPECTF--
Fatal error: Arrow functions on the right hand side of |> must be parenthesized in %s on line %d
