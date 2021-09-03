--TEST--
Bug #65947 (basename is no more working after fgetcsv in certain situation)
--FILE--
<?php
$filename = 'test.toto';
// é in ISO-8859-1
$csv = base64_decode('6Q==');
$adata = str_getcsv($csv,";");
$b2 = basename($filename);
if ($filename != $b2)
    print "BUG";
else
    print "OKEY";
?>
--EXPECT--
OKEY
