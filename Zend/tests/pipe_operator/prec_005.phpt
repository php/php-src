--TEST--
Pipe precedence 005
--FILE--
<?php

try {
    assert(false && 1 |> (fn() => 2));
} catch (AssertionError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
AssertionError: assert(false && 1 |> (fn() => 2))
