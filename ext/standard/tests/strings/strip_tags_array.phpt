--TEST--
Test strip_tags() function : basic functionality - with array argument
--FILE--
<?php

$string = '<p>foo <b>bar</b> <a href="#">foobar</a></p>';
var_dump(strip_tags($string));
var_dump(strip_tags($string, ['a']));
var_dump(strip_tags($string, ['p', 'a']));
var_dump(strip_tags($string, []));
var_dump(strip_tags($string, ['p' => true, 'a' => false]));
var_dump(strip_tags($string, ['p' => 'a']));

// Previous tests from strip_tags_variation2.phpt
var_dump(strip_tags($string, [0]));
var_dump(strip_tags($string, [1]));
var_dump(strip_tags($string, [1, 2]));
var_dump(strip_tags($string, ['color' => 'red', 'item' => 'pen']));
echo "Done";
?>
--EXPECT--
string(14) "foo bar foobar"
string(30) "foo bar <a href="#">foobar</a>"
string(37) "<p>foo bar <a href="#">foobar</a></p>"
string(14) "foo bar foobar"
string(14) "foo bar foobar"
string(30) "foo bar <a href="#">foobar</a>"
string(14) "foo bar foobar"
string(14) "foo bar foobar"
string(14) "foo bar foobar"
string(14) "foo bar foobar"
Done
