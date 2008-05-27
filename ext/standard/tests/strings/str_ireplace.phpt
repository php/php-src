--TEST--
str_ireplace() tests
--FILE--
<?php

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


echo "Done\n";
?>
--EXPECTF--
Warning: str_ireplace() expects at least 3 parameters, 0 given in %s on line %d
NULL

Warning: str_ireplace() expects at least 3 parameters, 1 given in %s on line %d
NULL

Warning: str_ireplace() expects at least 3 parameters, 2 given in %s on line %d
NULL
unicode(0) ""
unicode(8) "aaaaaaaT"
unicode(8) "aaaaaaaT"
int(7)
unicode(15) "aaaaaaaaaaaaaaT"
unicode(15) "aaaaaaaaaaaaaaT"
int(7)
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
int(7)
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
int(7)
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
array(2) {
  [0]=>
  unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
  [1]=>
  unicode(36) "aabbbbbbaabbbasdabbbbbbahsdbbbbbbbbb"
}
array(2) {
  [u"key"]=>
  unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
  [u"test"]=>
  unicode(36) "aabbbbbbaabbbasdabbbbbbahsdbbbbbbbbb"
}
array(2) {
  [u"key"]=>
  unicode(22) "aaaaaaaaaaaaaaaaaaaaaT"
  [u"test"]=>
  unicode(36) "aabbbbbbaabbbasdabbbbbbahsdbbbbbbbbb"
}
unicode(143) "Change tracking and management software designed to watch
	for abnormal system behavior.
Suggest features, report bugs, or ask
	questions here."
unicode(152) "Change tracking and management software designed to watch<br>	for abnormal system behavior.<br>Suggest features, report bugs, or ask<br>	questions here."
Done
