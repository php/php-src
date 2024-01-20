--TEST--
file_put_contents() with array as data
--FILE--
<?php

$file = __DIR__."/file_put_contents_array_as_data.txt";

$strings = [
    'hello',
    '',
    ' world',
    '!',
];

var_dump(file_put_contents($file, $strings));
var_dump(file_get_contents($file));

?>
--CLEAN--
<?php
$file = __DIR__."/file_put_contents_array_as_data.txt";
@unlink($file);
?>
--EXPECT--
int(12)
string(12) "hello world!"
