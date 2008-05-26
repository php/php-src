--TEST--
concat strings
--FILE--
<?php

$bin_s = (binary)"binary string";
$bin_s1 = (binary)"one more bin string";

$uni_s = "test string";
$uni_s1 = "one more test";

$tmp = $bin_s.$bin_s1;
var_dump($tmp);

$tmp = $bin_s.$uni_s;
var_dump($tmp);

$tmp = $uni_s.$uni_s1;
var_dump($tmp);

$tmp = $uni_s.$bin_s1;
var_dump($tmp);

echo "Done\n";
?>
--EXPECT--
string(32) "binary stringone more bin string"
unicode(24) "binary stringtest string"
unicode(24) "test stringone more test"
unicode(30) "test stringone more bin string"
Done
