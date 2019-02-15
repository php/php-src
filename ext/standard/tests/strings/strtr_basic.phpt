--TEST--
Test strtr() function : basic functionality
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
 * Description: Translates characters in str using given translation pairs
 * Source code: ext/standard/string.c
*/

echo "*** Testing strtr() : basic functionality ***\n";
//definitions of required input variables
$trans1_arr = array("t" => "T", "e" => "E", "st" => "ST");
$trans2_arr = array('t' => 'T', 'e' => 'E', 'st' => 'ST');
$heredoc_str = <<<EOD
test strtr
EOD;

//translating single char
var_dump( strtr("test strtr", "t", "T") );
var_dump( strtr('test strtr', 't', 'T') );
var_dump( strtr($heredoc_str, "t", "T") );

//translating set of chars
//$from and $to are of same length
var_dump( strtr("test strtr", "test", "TEST") );
var_dump( strtr('test strtr', 'test', 'TEST') );
var_dump( strtr($heredoc_str, "test", "TEST") );

//$from and $to are of different lengths, extra chars in the longer one are ignored
var_dump( strtr("test strtr", "test", "TESTz") );
var_dump( strtr('test strtr', 'testz', 'TEST') );
var_dump( strtr($heredoc_str, "test", "TESTz") );

//by using replace_pairs array
var_dump( strtr("test strtr", $trans1_arr) );
var_dump( strtr('test strtr', $trans2_arr) );
var_dump( strtr($heredoc_str, $trans1_arr) );
echo "*** Done ***";
?>
--EXPECT--
*** Testing strtr() : basic functionality ***
string(10) "TesT sTrTr"
string(10) "TesT sTrTr"
string(10) "TesT sTrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
string(10) "TEST STrTr"
*** Done ***
