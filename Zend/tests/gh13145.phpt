--TEST--
GH-13145: strtok() misoptimization
--FILE--
<?php
$tok = strtok("This is\tan example\nstring", " \n\t");
while ($tok !== false) {
    var_dump($tok);
    $tok = strtok(" \n\t");
}
?>
--EXPECT--
string(4) "This"
string(2) "is"
string(2) "an"
string(7) "example"
string(6) "string"
