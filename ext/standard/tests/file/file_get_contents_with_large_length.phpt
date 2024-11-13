--TEST--
Test file_get_contents() function with large length parameter
--INI--
memory_limit=128M
--FILE--
<?php

$file_path = __DIR__ . '/file_get_contents_with_large_length_content.txt';
$file_content = str_repeat('a', 50000);
file_put_contents($file_path, $file_content);

// test length limiting
$result = file_get_contents($file_path, length: 500000000);
var_dump($result === $file_content);

// test length limiting
$result = file_get_contents($file_path, length: 40000);
var_dump($result === str_repeat('a', 40000));

?>
--CLEAN--
<?php
unlink(__DIR__ . '/file_get_contents_with_large_length_content.txt');
?>
--EXPECT--
bool(true)
bool(true)
