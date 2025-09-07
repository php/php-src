--TEST--
GH-19752 (missing dup in phar_convert_to_other)
--EXTENSIONS--
phar
--FILE--
<?php
$phar = new PharData('test.1');

try {
    $phar->decompress("*");
} catch (Exception $ex) {
    var_dump($ex->getMessage());
}
?>
--EXPECTF--
string(%d) "data phar converted from "%stest.1" has invalid extension *"
