--TEST--
Bug #41815 (Concurrent read/write fails when EOF is reached)
--FILE--
<?php

$filename = dirname(__FILE__)."/concur_rw.txt";

@unlink($filename);
$writer = fopen($filename, "wt");
$reader = fopen($filename, "r");
fread($reader, 1);
fwrite($writer, "foo");

if (strlen(fread($reader, 10)) > 0) {
	echo "OK\n";
}

fclose($writer);
fclose($reader);

@unlink($filename);

echo "Done\n";
?>
--EXPECTF--	
OK
Done
