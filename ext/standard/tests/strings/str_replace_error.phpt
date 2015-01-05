--TEST--
Test str_replace() function error conditions
--INI--
precision=14
--FILE--
<?php
/* 
  Prototype: mixed str_replace(mixed $search, mixed $replace, 
                               mixed $subject [, int &$count]);
  Description: Replace all occurrences of the search string with 
               the replacement string
*/


echo "\n*** Testing str_replace error conditions ***";
/* Invalid arguments */
var_dump( str_replace() );
var_dump( str_replace("") );
var_dump( str_replace(NULL) );
var_dump( str_replace(1, 2) );
var_dump( str_replace(1,2,3,$var,5) );

?>
===DONE===
--EXPECTF--	
*** Testing str_replace error conditions ***
Warning: str_replace() expects at least 3 parameters, 0 given in %sstr_replace_error.php on line 12
NULL

Warning: str_replace() expects at least 3 parameters, 1 given in %sstr_replace_error.php on line 13
NULL

Warning: str_replace() expects at least 3 parameters, 1 given in %sstr_replace_error.php on line 14
NULL

Warning: str_replace() expects at least 3 parameters, 2 given in %sstr_replace_error.php on line 15
NULL

Warning: str_replace() expects at most 4 parameters, 5 given in %sstr_replace_error.php on line 16
NULL
===DONE===