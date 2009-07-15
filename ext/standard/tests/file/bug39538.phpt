--TEST--
Bug #39538 (fgetcsv can't handle starting newlines and trailing odd number of backslashes)
--FILE--
<?php
$content = array("\"\nthis is an test\", \"next data\", \"p\narsed\"","\"\r\nthis is an test\", \"next data\", \"p\r\narsed\"","\"\n\rthis is an test\", \"next data\", \"p\n\rarsed\"");

$file = dirname(__FILE__) . "/bug39538.csv";
@unlink($file);
foreach ($content as $v) {
	file_put_contents($file, $v);
	print_r (fgetcsv(fopen($file, "r"), filesize($file)));
}
@unlink($file);
?>
--EXPECT--
Array
(
    [0] => 
this is an test
    [1] => next data
    [2] => p
arsed
)
Array
(
    [0] => 
this is an test
    [1] => next data
    [2] => p
arsed
)
Array
(
    [0] => 
this is an test
    [1] => next data
    [2] => p
arsed
)
