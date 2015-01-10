--TEST--
str_ireplace() tests
--FILE--
<?php
function check_ireplace($search,$replace,&$subject)
{
$c=0;
var_dump($subject=str_ireplace($search,$replace,$subject,$c));
echo "Count: $c\n";
}

//------

var_dump(str_ireplace());
var_dump(str_ireplace(""));
var_dump(str_ireplace("", ""));
var_dump(str_ireplace("", "", ""));

var_dump(str_ireplace("tt", "a", "ttttTttttttttTT"));
var_dump(str_ireplace("tt", "a", "ttttTttttttttTT", $count));
var_dump($count);

var_dump(str_ireplace("tt", "aa", "ttttTttttttttTT"));
var_dump(str_ireplace("tt", "aa", "ttttTttttttttTT", $count));
var_dump($count);

var_dump(str_ireplace("tt", "aaa", "ttttTttttttttTT"));
var_dump(str_ireplace("tt", "aaa", "ttttTttttttttTT", $count));
var_dump($count);

var_dump(str_ireplace("tt", "aaa", "ttttTttttttttTT"));
var_dump(str_ireplace("tt", "aaa", "ttttTttttttttTT", $count));
var_dump($count);

var_dump(str_ireplace(array("tt", "tt"), "aaa", "ttttTttttttttTT"));
var_dump(str_ireplace(array("tt", "tt"), array("aaa"), "ttttTttttttttTT"));
var_dump(str_ireplace(array("tt", "y"), array("aaa", "bbb"), "ttttTttttttttTT"));

var_dump(str_ireplace(array("tt", "tt"), "aaa", "ttttTttttttttTT"));
var_dump(str_ireplace(array("tt", "tt"), array("aaa"), "ttttTttttttttTT"));
var_dump(str_ireplace(array("tt", "y"), array("aaa", "bbb"), "ttttTttttttttTT"));

var_dump(str_ireplace(array("tt", "y"), array("aaa", "bbb"), array("ttttTttttttttTT", "aayyaayasdayYahsdYYY")));
var_dump(str_ireplace(array("tt", "y"), array("aaa", "bbb"), array("key"=>"ttttTttttttttTT", "test"=>"aayyaayasdayYahsdYYY")));
var_dump(str_ireplace(array("t"=>"tt", "y"=>"y"), array("a"=>"aaa", "b"=>"bbb"), array("key"=>"ttttTttttttttTT", "test"=>"aayyaayasdayYahsdYYY")));

/* separate testcase for str_ireplace() off-by-one */

$Data = "Change tracking and management software designed to watch
	for abnormal system behavior.\nSuggest features, report bugs, or ask
	questions here.";
var_dump($Data = str_ireplace("\r\n", "<br>", $Data));
var_dump($Data = str_ireplace("\n", "<br>", $Data));

/* Check 'needle=string/replace=array' case */

$search='a';
$repl=array('B','C','A',10,'Aab', null);
$subject='FgAnBraVAAaNBaFjAaaAaJzK?';
check_ireplace($search,$repl,$subject);
check_ireplace($search,$repl,$subject);

$repl=array();
$subject='FgAnBraVAAaNBaFjAaaAaJzK?';
check_ireplace($search,$repl,$subject);

echo "Done\n";
?>
--EXPECTF--	
Warning: str_ireplace() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: str_ireplace() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: str_ireplace() expects at least 3 parameters, 2 given in %s on line %d
NULL
string(0) ""
string(8) "aaaaaaaT"
string(8) "aaaaaaaT"
int(7)
string(15) "aaaaaaaaaaaaaaT"
string(15) "aaaaaaaaaaaaaaT"
int(7)
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
int(7)
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
int(7)
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
string(22) "aaaaaaaaaaaaaaaaaaaaaT"
array(2) {
  [0]=>
  string(22) "aaaaaaaaaaaaaaaaaaaaaT"
  [1]=>
  string(36) "aabbbbbbaabbbasdabbbbbbahsdbbbbbbbbb"
}
array(2) {
  ["key"]=>
  string(22) "aaaaaaaaaaaaaaaaaaaaaT"
  ["test"]=>
  string(36) "aabbbbbbaabbbasdabbbbbbahsdbbbbbbbbb"
}
array(2) {
  ["key"]=>
  string(22) "aaaaaaaaaaaaaaaaaaaaaT"
  ["test"]=>
  string(36) "aabbbbbbaabbbasdabbbbbbahsdbbbbbbbbb"
}
string(143) "Change tracking and management software designed to watch
	for abnormal system behavior.
Suggest features, report bugs, or ask
	questions here."
string(152) "Change tracking and management software designed to watch<br>	for abnormal system behavior.<br>Suggest features, report bugs, or ask<br>	questions here."
string(27) "FgBnBrCVA10AabNBFjAaaAaJzK?"
Count: 6
string(29) "FgBnBrCVB10CAbNBFj10AabAaJzK?"
Count: 6

Warning: str_ireplace(): Replace array must not be empty in %s on line %d
string(25) "FgAnBraVAAaNBaFjAaaAaJzK?"
Count: 0
Done
