--TEST--
Stream: RFC2397 Mozilla tests
--INI--
allow_url_fopen=1
--FILE--
<?php

$streams = array(
    'data://,;test',
    'data://text/plain,test',
    'data://text/plain;charset=US-ASCII,test',
    'data://;charset=UTF-8,Hello',
    'data://text/plain;charset=UTF-8,Hello',
    'data://,a,b',
    );

foreach($streams as $stream)
{
    var_dump(@file_get_contents($stream));
}

?>
--EXPECT--
string(5) ";test"
string(4) "test"
string(4) "test"
bool(false)
string(5) "Hello"
string(3) "a,b"
