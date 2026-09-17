--TEST--
Bug #71952 (Corruption inside imageaffinematrixget)
--EXTENSIONS--
gd
--FILE--
<?php
$vals=[str_repeat("A","200"),0,1,2,3,4,5,6,7,8,9];
try {
    imageaffinematrixget(4, $vals[0]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump($vals[0]);
?>
--EXPECT--
TypeError: imageaffinematrixget(): Argument #2 ($options) must be of type array|float, string given
string(200) "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
