--TEST--
iconv test
--SKIPIF--
<?php if (!extension_loaded("iconv")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
echo "iconv extension is available";
$test = "Stig S誥her Bakken";
print "$test\n";
printf("%s\n", iconv("iso-8859-1", "utf-8", $test));

?>
--EXPECT--
iconv extension is available
Stig S誥her Bakken
Stig Sﾃｦther Bakken
