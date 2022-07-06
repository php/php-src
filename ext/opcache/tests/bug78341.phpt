--TEST--
Bug #78341: Failure to detect smart branch in DFA pass
--FILE--
<?php

function test($a) {
    // Just some dead code...
    if (strpos("foo", "foo") !== 0) {
        echo "Foo";
    }

    $x = $a === null;
    if ($x) {
        var_dump($x);
    }
}
test(null);

?>
--EXPECT--
bool(true)
