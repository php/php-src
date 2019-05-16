--TEST--
Bug #75574 putenv does not work properly if parameter contains non-ASCII unicode character, UTF-8
--INI--
internal_encoding=utf-8
--FILE--
<?php
/*
#vim: set fileencoding=utf-8
#vim: set encoding=utf-8
*/

var_dump(putenv('FOO=啊'), getenv("FOO"));
var_dump(putenv('FOO=啊啊'), getenv("FOO"));
var_dump(putenv('FOO=啊啊啊'), getenv("FOO"));
var_dump(putenv('FOO=啊啊啊啊'), getenv("FOO"));

var_dump(putenv('FOO=啊a'), getenv("FOO"));
var_dump(putenv('FOO=啊a啊'), getenv("FOO"));
var_dump(putenv('FOO=啊a啊a'), getenv("FOO"));
var_dump(putenv('FOO=啊a啊a啊'), getenv("FOO"));
var_dump(putenv('FOO=啊a啊啊'), getenv("FOO"));
var_dump(putenv('FOO=啊a啊啊啊'), getenv("FOO"));
var_dump(putenv('FOO=啊a啊啊啊啊'), getenv("FOO"));

?>
===DONE===
--EXPECT--
bool(true)
string(3) "啊"
bool(true)
string(6) "啊啊"
bool(true)
string(9) "啊啊啊"
bool(true)
string(12) "啊啊啊啊"
bool(true)
string(4) "啊a"
bool(true)
string(7) "啊a啊"
bool(true)
string(8) "啊a啊a"
bool(true)
string(11) "啊a啊a啊"
bool(true)
string(10) "啊a啊啊"
bool(true)
string(13) "啊a啊啊啊"
bool(true)
string(16) "啊a啊啊啊啊"
===DONE===
