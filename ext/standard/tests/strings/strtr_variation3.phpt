--TEST--
Test strtr() function : usage variations - string containing escape sequences for 'str' argument
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Testing strtr() function by passing the
 *   string containing various escape sequences for 'str' argument and
 *   corresponding translation pair of chars for 'from', 'to' & 'replace_pairs' arguments
*/

echo "*** Testing strtr() : string containing escape sequences for 'str' arg ***\n";
/* definitions of required input variables */
$count = 1;

$heredoc_str = <<<EOD
\tes\t\\stt\r
\\test\\\strtr
\ntest\r\nstrtr
\$variable
\"quotes
EOD;

//array of string inputs for $str
$str_arr = array(
  //double quoted strings
  "\tes\t\\stt\r",
  "\\test\\\strtr",
  "\ntest\r\nstrtr",
  "\$variable",
  "\"quotes",

  //single quoted strings
  '\tes\t\\stt\r',
  '\\test\\\strtr',
  '\ntest\r\nstrtr',
  '\$variable',
  '\"quotes',

  //heredoc string
  $heredoc_str
);

$from = "\n\r\t\\";
$to = "TEST";
$replace_pairs = array("\n" => "t", "\r\n" => "T", "\n\r\t\\" => "TEST");

/* loop through to test strtr() with each element of $str_arr */
for($index = 0; $index < count($str_arr); $index++) {
  echo "-- Iteration $count --\n";

  $str = $str_arr[$index];  //getting the array element in 'str' variable

  //strtr() call in three args syntax form
  var_dump( strtr($str, $from, $to) );

  //strtr() call in two args syntax form
  var_dump( strtr($str, $replace_pairs) );

  $count++;
}
echo "*** Done ***";
?>
--EXPECTF--
*** Testing strtr() : string containing escape sequences for 'str' arg ***
-- Iteration 1 --
string(9) "SesSTsttE"
string(9) "	es	\stt"
-- Iteration 2 --
string(12) "TtestTTstrtr"
string(12) "\test\\strtr"
-- Iteration 3 --
string(12) "TtestETstrtr"
string(11) "ttestTstrtr"
-- Iteration 4 --
string(9) "$variable"
string(9) "$variable"
-- Iteration 5 --
string(7) ""quotes"
string(7) ""quotes"
-- Iteration 6 --
string(12) "TtesTtTsttTr"
string(12) "\tes\t\stt\r"
-- Iteration 7 --
string(12) "TtestTTstrtr"
string(12) "\test\\strtr"
-- Iteration 8 --
string(15) "TntestTrTnstrtr"
string(15) "\ntest\r\nstrtr"
-- Iteration 9 --
string(10) "T$variable"
string(10) "\$variable"
-- Iteration 10 --
string(8) "T"quotes"
string(8) "\"quotes"
-- Iteration 11 --
string(54) "SesSTsttETTtestTTstrtrTTtestETstrtrT$variableTT"quotes"
string(52) "	es	\sttT\test\\strtrtttestTstrtrt$variablet\"quotes"
*** Done ***