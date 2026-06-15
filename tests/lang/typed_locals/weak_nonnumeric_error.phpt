--TEST--
Typed local variables: weak mode, non-numeric string to int throws TypeError
--FILE--
<?php
int $x = 1;
try {
    $x = "abc";
} catch (\TypeError $e) {
    echo "caught\n";
}
?>
--EXPECT--
caught
