--TEST--
enable_post_data_reading: seeking in php://input
--INI--
enable_post_data_reading=1
--POST_RAW--
Content-Type: application/unknown
0123456789
--FILE--
<?php
echo "Test\n";

$f1 = fopen("php://input", "r");
fseek($f1, 3, SEEK_SET);
echo fgetc($f1);
fseek($f1, 1, SEEK_SET);
echo fgetc($f1);
fseek($f1, 3, SEEK_CUR);
echo fgetc($f1);
fseek($f1, -3, SEEK_CUR);
echo fgetc($f1);
fseek($f1, 3, SEEK_END);
echo fgetc($f1);
fseek($f1, -3, SEEK_END);
$f2 = fopen("php://input", "r");
fseek($f2, 1, SEEK_SET);
echo fgetc($f1);
echo fgetc($f2);
?>

Done
--EXPECT--
Test
315371
Done
