--TEST--
Bug #40909 (pcre 7.0 regression)
--FILE--
<?php

$pattern =
"/\s([\w_\.\/]+)(?:=([\'\"]?(?:[\w\d\s\?=\(\)\.,'_#\/\\:;&-]|(?:\\\\\"|\\\')?)+[\'\"]?))?/";
$context = "<simpletag an_attribute=\"simpleValueInside\">";

$match = array();

if ($result =preg_match_all($pattern, $context, $match))
{

var_dump($result);
var_dump($match);
}

?>
--EXPECT--
int(1)
array(3) {
  [0]=>
  array(1) {
    [0]=>
    string(33) " an_attribute="simpleValueInside""
  }
  [1]=>
  array(1) {
    [0]=>
    string(12) "an_attribute"
  }
  [2]=>
  array(1) {
    [0]=>
    string(19) ""simpleValueInside""
  }
}
