--TEST--
Pipe precedence 002
--FILE--
<?php

42
    |> (fn($x) => $x < 42)
    |> (fn($x) => var_dump($x));

?>
--EXPECT--
bool(false)
