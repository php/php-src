--TEST--
Stream: RFC2397 without //
--INI--
allow_url_fopen=1
--FILE--
<?php

$streams = array(
	'data:,A%20brief%20note',
	'data:application/vnd-xxx-query,select_vcount,fcol_from_fieldtable/local',
	'data:;base64,Zm9vYmFyIGZvb2Jhcg==',
	'data:,;test',
	'data:text/plain,test',
	'data:text/plain;charset=US-ASCII,test',
	'data:;charset=UTF-8,Hello',
	'data:text/plain;charset=UTF-8,Hello',
	'data:,a,b',
	);

foreach($streams as $stream)
{
	var_dump(@file_get_contents($stream));
}

?>
===DONE===
<?php exit(0); ?>
--EXPECT--
string(12) "A brief note"
string(40) "select_vcount,fcol_from_fieldtable/local"
string(13) "foobar foobar"
string(5) ";test"
string(4) "test"
string(4) "test"
bool(false)
string(5) "Hello"
string(3) "a,b"
===DONE===
