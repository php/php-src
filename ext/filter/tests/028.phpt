--TEST--
filter_data() and FS_SPECIAL_CHARS
--FILE--
<?php

var_dump(filter_data("?><!@#$%^&*()}{~Qwertyuilfdsasdfgmnbvcxcvbn", FS_SPECIAL_CHARS));
var_dump(filter_data("<data&sons>", FS_SPECIAL_CHARS));
var_dump(filter_data("", FS_SPECIAL_CHARS));

var_dump(filter_data("?><!@#$%^&*()}{~Qwertyuilfdsasdfgmnbvcxcvbn", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_LOW));
var_dump(filter_data("<data&sons>", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_LOW));
var_dump(filter_data("", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_LOW));

var_dump(filter_data("?><!@#$%^&*()}{~Qwertyuilfdsasdfgmnbvcxcvbn", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_HIGH));
var_dump(filter_data("<data&sons>", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_HIGH));
var_dump(filter_data("", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_HIGH));

var_dump(filter_data("кириллица", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_HIGH));
var_dump(filter_data("кириллица", FS_SPECIAL_CHARS, FILTER_FLAG_ENCODE_LOW));

echo "Done\n";
?>
--EXPECT--	
string(55) "?&#62;&#60;!@#$%^&#38;*()}{~Qwertyuilfdsasdfgmnbvcxcvbn"
string(23) "&#60;data&#38;sons&#62;"
string(0) ""
string(55) "?&#62;&#60;!@#$%^&#38;*()}{~Qwertyuilfdsasdfgmnbvcxcvbn"
string(23) "&#60;data&#38;sons&#62;"
string(0) ""
string(55) "?&#62;&#60;!@#$%^&#38;*()}{~Qwertyuilfdsasdfgmnbvcxcvbn"
string(23) "&#60;data&#38;sons&#62;"
string(0) ""
string(108) "&#208;&#186;&#208;&#184;&#209;&#128;&#208;&#184;&#208;&#187;&#208;&#187;&#208;&#184;&#209;&#134;&#208;&#176;"
string(18) "кириллица"
Done
