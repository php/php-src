--TEST--
Bug #12556: fgetcvs ignores lengths when quotes not closed
--POST--
--GET--
--FILE--
<?php
$fp=fopen(dirname(__FILE__)."/test.csv", "r");
while($line=fgetcsv($fp, 24)){
	print("Read 24 bytes\n");
}
?>
--EXPECT--
Read 24 bytes
Read 24 bytes
Read 24 bytes
Read 24 bytes
Read 24 bytes
Read 24 bytes
Read 24 bytes
Read 24 bytes
Read 24 bytes
