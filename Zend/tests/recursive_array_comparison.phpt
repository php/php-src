--TEST--
Comparison of a recursive array throws a catchable error
--FILE--
<?php
$a = [&$a];
try {
    $a === [[]];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    [[]] === $a;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($a === $a);
?>
--EXPECT--
Nesting level too deep - recursive dependency?
Nesting level too deep - recursive dependency?
bool(true)
