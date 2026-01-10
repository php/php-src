--TEST--
Pipe precedence 005
--FILE--
<?php

try {
    assert(false && 1 |> (fn() => 2));
} catch (AssertionError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
assert(false && 1 |> (fn() => 2))
