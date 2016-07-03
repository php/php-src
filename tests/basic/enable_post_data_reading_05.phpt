--TEST--
enable_post_data_reading: using multiple input streams
--INI--
enable_post_data_reading=0
max_execution_time=2
--POST_RAW--
Content-Type: application/unknown
One line of data
--FILE--
<?php
echo "Test\n";

$f1 = fopen("php://input", "r");
$f2 = fopen("php://input", "r");

while (!feof($f1) && !feof($f2)) {
	echo fgetc($f1), fgetc($f2);
}

?>

Done
--EXPECT--
Test
OOnnee  lliinnee  ooff  ddaattaa
Done
