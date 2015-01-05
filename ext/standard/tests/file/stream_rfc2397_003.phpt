--TEST--
Stream: RFC2397 decoding data
--INI--
allow_url_fopen=1
--FILE--
<?php

$streams = array(
	'data://,A%20brief%20note',
	'data://application/vnd-xxx-query,select_vcount,fcol_from_fieldtable/local',
	'data://;base64,Zm9vYmFyIGZvb2Jhcg==',
	'stream_rfc2397_003.gif' => 'data://image/gif;base64,R0lGODdhMAAwAPAAAAAAAP///ywAAAAAMAAw
AAAC8IyPqcvt3wCcDkiLc7C0qwyGHhSWpjQu5yqmCYsapyuvUUlvONmOZtfzgFz
ByTB10QgxOR0TqBQejhRNzOfkVJ+5YiUqrXF5Y5lKh/DeuNcP5yLWGsEbtLiOSp
a/TPg7JpJHxyendzWTBfX0cxOnKPjgBzi4diinWGdkF8kjdfnycQZXZeYGejmJl
ZeGl9i2icVqaNVailT6F5iJ90m6mvuTS4OK05M0vDk0Q4XUtwvKOzrcd3iq9uis
F81M1OIcR7lEewwcLp7tuNNkM3uNna3F2JQFo97Vriy/Xl4/f1cf5VWzXyym7PH
hhx4dbgYKAAA7',
	);

foreach($streams as $original => $stream)
{
	if (is_string($original)) {
		var_dump(file_get_contents(dirname(__FILE__) . '/' . $original) == file_get_contents($stream));
	} else {
		var_dump(file_get_contents($stream));
	}
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
string(12) "A brief note"
string(40) "select_vcount,fcol_from_fieldtable/local"
string(13) "foobar foobar"
bool(true)
===DONE===
