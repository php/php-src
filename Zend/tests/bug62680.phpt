--TEST--
Bug #62680 (Function isset() throws fatal error on set array if non-existent key depth >= 3)
--FILE--
<?php
$array = array("");
try {
    var_dump(isset($array[0]["a"]["b"]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
try {
    var_dump(isset($array[0]["a"]["b"]["c"]));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
Cannot access offset of type string on string
Cannot access offset of type string on string
