--TEST--
Test strtr() function : usage variations - empty string & null for 'str' argument
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Testing strtr() function by passing the
 *   empty string & null for 'str' argument and
 *   corresponding translation pair of chars for 'from', 'to' & 'replace_pairs' arguments
*/

echo "*** Testing strtr() : empty string & null for 'str' arg ***\n";
/* definitions of required input variables */
$count = 1;

$heredoc_str = <<<EOD

EOD;

//array of string inputs for $str
$str_arr = array(
  "",
  '',
  NULL,
  null,
  FALSE,
  false,
  $heredoc_str
);

$from = "";
$to = "TEST";
$replace_pairs = array("" => "t", '' => "TEST");


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
--EXPECT--
*** Testing strtr() : empty string & null for 'str' arg ***
-- Iteration 1 --
string(0) ""
string(0) ""
-- Iteration 2 --
string(0) ""
string(0) ""
-- Iteration 3 --
string(0) ""
string(0) ""
-- Iteration 4 --
string(0) ""
string(0) ""
-- Iteration 5 --
string(0) ""
string(0) ""
-- Iteration 6 --
string(0) ""
string(0) ""
-- Iteration 7 --
string(0) ""
string(0) ""
*** Done ***
