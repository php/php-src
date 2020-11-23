--TEST--
Stream: RFC2397 with corrupt? payload
--INI--
allow_url_fopen=1
--FILE--
<?php

$streams = array(
    "data:;base64,\0Zm9vYmFyIGZvb2Jhcg==",
    "data:;base64,Zm9vYmFy\0IGZvb2Jhcg==",
    'data:;base64,#Zm9vYmFyIGZvb2Jhcg==',
    'data:;base64,#Zm9vYmFyIGZvb2Jhc=',
    );

foreach($streams as $stream)
{
    try {
        var_dump(file_get_contents($stream));
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
file_get_contents(): Argument #1 ($filename) must not contain any null bytes
file_get_contents(): Argument #1 ($filename) must not contain any null bytes

Warning: file_get_contents(data:;base64,#Zm9vYmFyIGZvb2Jhcg==): Failed to open stream: rfc2397: unable to decode in %sstream_rfc2397_006.php on line %d
bool(false)

Warning: file_get_contents(data:;base64,#Zm9vYmFyIGZvb2Jhc=): Failed to open stream: rfc2397: unable to decode in %sstream_rfc2397_006.php on line %d
bool(false)
