--TEST--
Bug #39538 (fgetcsv can't handle starting newlines and trailing odd number of backslashes)
--FILE--
<?php

$file = __DIR__ . "/bug39538.csv";

$line = "\"\nthis is a test\", \"next data\", \"p\narsed\"";
$expectation = [
    "\nthis is a test",
    "next data",
    "p\narsed",
];

file_put_contents($file, $line);
$parsed_content = fgetcsv(fopen($file, "r"), filesize($file), escape: "\\");
var_dump($parsed_content === $expectation);

@unlink($file);

$line = "\"\r\nthis is a test\", \"next data\", \"p\r\narsed\"";
$expectation = [
    "\r\nthis is a test",
    "next data",
    "p\r\narsed",
];

file_put_contents($file, $line);
$parsed_content = fgetcsv(fopen($file, "r"), filesize($file), escape: "\\");
var_dump($parsed_content === $expectation);

@unlink($file);

$line = "\"\n\rthis is a test\", \"next data\", \"p\n\rarsed\"";
$expectation = [
    "\n\rthis is a test",
    "next data",
    "p\n\rarsed",
];

file_put_contents($file, $line);
$parsed_content = fgetcsv(fopen($file, "r"), filesize($file), escape: "\\");
var_dump($parsed_content === $expectation);

?>
--CLEAN--
<?php
$file = __DIR__ . "/bug39538.csv";
@unlink($file);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
