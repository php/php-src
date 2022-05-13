--TEST--
Bug #64343 (phar cannot open tars with pax headers)
--EXTENSIONS--
phar
--FILE--
<?php

echo "Test\n";

$phar = new PharData(__DIR__."/files/bug64343.tar");

?>
--EXPECT--
Test
