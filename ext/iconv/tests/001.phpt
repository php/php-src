--TEST--
iconv test
--SKIPIF--
<?php if (!extension_loaded("iconv")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
echo "iconv extension is available\n";
$test = "æøå";
printf("ISO-8859-1: %s\nUTF-8: %s\n",
       $test, iconv("ISO-8859-1", "UTF-8", $test));

?>
--EXPECT--
iconv extension is available
ISO-8859-1: æøå
UTF-8: Ã¦Ã¸Ã¥
