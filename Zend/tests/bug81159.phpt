--TEST--
Bug #81159: Object to int warning when using an object as a string offset
--FILE--
<?php

$s = 'Hello';
$o = new stdClass();
try {
    $s[$o] = 'A';
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump($s[$o]);
} catch (\Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot access offset of type stdClass on string
Cannot access offset of type stdClass on string
