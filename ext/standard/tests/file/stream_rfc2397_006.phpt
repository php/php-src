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
	var_dump(file_get_contents($stream));
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Warning: file_get_contents() expects parameter 1 to be a valid path, string given in %s line %d
NULL

Warning: file_get_contents() expects parameter 1 to be a valid path, string given in %s line %d
NULL
string(13) "foobar foobar"

Warning: file_get_contents(data:;base64,#Zm9vYmFyIGZvb2Jhc=): failed to open stream: rfc2397: unable to decode in %sstream_rfc2397_006.php on line %d
bool(false)
===DONE===
