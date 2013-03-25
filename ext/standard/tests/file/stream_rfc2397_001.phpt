--TEST--
Stream: RFC2397 getting the data
--INI--
allow_url_fopen=1
--FILE--
<?php

$data = 'data://,hello world';

var_dump(file_get_contents($data));

$file = fopen($data, 'r');
unset($data);

var_dump(stream_get_contents($file));

?>
===DONE===
--EXPECT--
string(11) "hello world"
string(11) "hello world"
===DONE===