--TEST--
Test fpassthru() function with length
--FILE--
<?php
$file_name = __DIR__."/passthru_with_length.tmp";
$file_write = fopen($file_name, "w");
fwrite($file_write, "1234567890abcdefghijklmnopqrstuvwxyz");
fclose($file_write);

$file_read = fopen($file_name, "r");

echo "Test consecutive reads\n";
$read = [];
$read[] = fpassthru($file_read, 1);
echo "\n";
$read[] = fpassthru($file_read, 3);
echo "\n";
$read[] = fpassthru($file_read, 0);
echo "\n";
$read[] = fpassthru($file_read, 1);
echo "\n";
$read[] = fpassthru($file_read);
echo "\n";
print_r($read);

echo "Read full file after rewind\n";
rewind($file_read);
fpassthru($file_read);
echo "\n";

echo "Test reading at file offsets\n";
$read = [];
fseek($file_read, 10);
$read[] = fpassthru($file_read, 5);
echo "\n";
fseek($file_read, -5, SEEK_END);
$read[] = fpassthru($file_read, 1);
echo "\n";
fseek($file_read, -3, SEEK_END);
$read[] = fpassthru($file_read, 1000);
echo "\n";
$read[] = fpassthru($file_read);
echo "\n";
print_r($read);

echo "Test reading negative lengths\n";
$read = [];
rewind($file_read);
$read[] = fpassthru($file_read, -1);
echo "\n";
rewind($file_read);
$read[] = fpassthru($file_read, -2);
echo "\n";
rewind($file_read);
$read[] = fpassthru($file_read, -100);
echo "\n";
rewind($file_read);
$read[] = fpassthru($file_read, PHP_INT_MIN);
echo "\n";
print_r($read);

fclose($file_read);
?>
--EXPECT--
Test consecutive reads
1
234

5
67890abcdefghijklmnopqrstuvwxyz
Array
(
    [0] => 1
    [1] => 3
    [2] => 0
    [3] => 1
    [4] => 31
)
Read full file after rewind
1234567890abcdefghijklmnopqrstuvwxyz
Test reading at file offsets
abcde
v
xyz

Array
(
    [0] => 5
    [1] => 1
    [2] => 3
    [3] => 0
)
Test reading negative lengths
1234567890abcdefghijklmnopqrstuvwxyz
1234567890abcdefghijklmnopqrstuvwxyz
1234567890abcdefghijklmnopqrstuvwxyz
1234567890abcdefghijklmnopqrstuvwxyz
Array
(
    [0] => 36
    [1] => 36
    [2] => 36
    [3] => 36
)

--CLEAN--
<?php
@unlink(__DIR__."/passthru_with_length.tmp");
?>