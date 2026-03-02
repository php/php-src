--TEST--
Test chop() function : usage variations - strings with embedded nulls
--FILE--
<?php
/*
 * Testing chop() : with nulls embedded in input string
*/

echo "*** Testing chop() : string with embedded nulls ***\n";

// defining varous strings with embedded nulls
$strings_with_nulls = array(
               "hello\0world",
               "\0hello",
               "hello\0",
               "\0\0hello\tworld\0\0",
               "\\0hello\\0",
               'hello\0\0',
               chr(0),
               chr(0).chr(0),
                   chr(0).'hello'.chr(0),
               'hello'.chr(0).'world'
               );

$count = 1;
foreach($strings_with_nulls as $string)  {
  echo "\n--- Iteration $count ---\n";
  var_dump( chop($string) );
  var_dump( chop($string, "\0") );
  var_dump( chop($string, '\0') );
  $count++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing chop() : string with embedded nulls ***

--- Iteration 1 ---
string(11) "hello%0world"
string(11) "hello%0world"
string(11) "hello%0world"

--- Iteration 2 ---
string(6) "%0hello"
string(6) "%0hello"
string(6) "%0hello"

--- Iteration 3 ---
string(5) "hello"
string(5) "hello"
string(6) "hello%0"

--- Iteration 4 ---
string(13) "%0%0hello	world"
string(13) "%0%0hello	world"
string(15) "%0%0hello	world%0%0"

--- Iteration 5 ---
string(9) "\0hello\0"
string(9) "\0hello\0"
string(7) "\0hello"

--- Iteration 6 ---
string(9) "hello\0\0"
string(9) "hello\0\0"
string(5) "hello"

--- Iteration 7 ---
string(0) ""
string(0) ""
string(1) "%0"

--- Iteration 8 ---
string(0) ""
string(0) ""
string(2) "%0%0"

--- Iteration 9 ---
string(6) "%0hello"
string(6) "%0hello"
string(7) "%0hello%0"

--- Iteration 10 ---
string(11) "hello%0world"
string(11) "hello%0world"
string(11) "hello%0world"
Done
