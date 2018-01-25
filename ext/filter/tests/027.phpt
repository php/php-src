--TEST--
filter_var() and FILTER_SANITIZE_ENCODED
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

var_dump(filter_var("?><!@#$%^&*()}{~Qwertyuilfdsasdfgmnbvcxcvbn", FILTER_SANITIZE_ENCODED));
var_dump(filter_var("<data&sons>", FILTER_SANITIZE_ENCODED));
var_dump(filter_var("", FILTER_SANITIZE_ENCODED));

var_dump(filter_var("?><!@#$%^&*()}{~Qwertyuilfdsasdfgmnbvcxcvbn", FILTER_SANITIZE_ENCODED, FILTER_FLAG_ENCODE_LOW));
var_dump(filter_var("<data&sons>", FILTER_SANITIZE_ENCODED, FILTER_FLAG_ENCODE_LOW));
var_dump(filter_var("", FILTER_SANITIZE_ENCODED, FILTER_FLAG_ENCODE_LOW));

var_dump(filter_var("?><!@#$%^&*()}{~Qwertyuilfdsasdfgmnbvcxcvbn", FILTER_SANITIZE_ENCODED, FILTER_FLAG_ENCODE_HIGH));
var_dump(filter_var("<data&sons>", FILTER_SANITIZE_ENCODED, FILTER_FLAG_ENCODE_HIGH));
var_dump(filter_var("", FILTER_SANITIZE_ENCODED, FILTER_FLAG_ENCODE_HIGH));

echo "Done\n";
?>
--EXPECT--
string(75) "%3F%3E%3C%21%40%23%24%25%5E%26%2A%28%29%7D%7B%7EQwertyuilfdsasdfgmnbvcxcvbn"
string(17) "%3Cdata%26sons%3E"
string(0) ""
string(75) "%3F%3E%3C%21%40%23%24%25%5E%26%2A%28%29%7D%7B%7EQwertyuilfdsasdfgmnbvcxcvbn"
string(17) "%3Cdata%26sons%3E"
string(0) ""
string(75) "%3F%3E%3C%21%40%23%24%25%5E%26%2A%28%29%7D%7B%7EQwertyuilfdsasdfgmnbvcxcvbn"
string(17) "%3Cdata%26sons%3E"
string(0) ""
Done
