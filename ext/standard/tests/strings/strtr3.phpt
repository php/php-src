--TEST--
strtr() function (windown-1251 encoding)
--INI--
unicode.script_encoding=windows-1251
unicode.output_encoding=windows-1251
--FILE--
<?php
$trans = array("дрова"=>"трава", "трава"=>"дрова", "а"=>"А", "под ними"=>"на траве");
var_dump(strtr("# На дворе дрова, под ними трава. #", $trans));
?>
--EXPECT--
string(35) "# НА дворе трава, на траве дрова. #"
--UEXPECT--
unicode(35) "# НА дворе трава, на траве дрова. #"
