--TEST--
Test chunk_split() function : usage variations - different heredoc strings for 'ending' argument 
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* passing different heredoc strings as 'ending' argument to chunk_split() 
* 'chunklen' argument is set to 10
*/

echo "*** Testing chunk_split() : different heredoc strings for 'ending' argument ***\n";

// Initializing required variables
$chunklen = 10;
$str = "This is str to check with heredoc ending.This\tcontains,\nspeci@! ch@r$ __with wrong \k escape char 222.";

// Null heredoc string
$heredoc_null = <<<EOT1
EOT1;

// heredoc string with single character
$heredoc_char = <<<EOT2
a
EOT2;

// simple heredoc string
$heredoc_str = <<<EOT3
This is simple heredoc string
EOT3;

// heredoc with special characters
$heredoc_spchar = <<<EOT4
This checks with $, %, &, chars
EOT4;

// blank heredoc string
$heredoc_blank = <<<EOT5

EOT5;

// heredoc with different white space characters
$heredoc_escchar = <<<EOT6
This checks\t and \nwhite space chars
EOT6;

// heredoc with multiline
$heredoc_multiline= <<<EOT7
This is to check chunk_split
function with multiline
heredoc
EOT7;

// heredoc with quotes and slashes
$heredoc_quote_slash = <<<EOT8
"To check " in heredoc".I'm sure it'll \work!
EOT8;

// different heredoc strings for 'ending'
$heredoc_arr = array(
  $heredoc_null,
  $heredoc_blank,
  $heredoc_char,
  $heredoc_str,
  $heredoc_multiline,
  $heredoc_spchar,
  $heredoc_escchar,
  $heredoc_quote_slash
);


// loop through each element of the heredoc_arr for str
$count = 0;
foreach($heredoc_arr as $value) {
  echo "-- Iteration ".($count+1). " --\n";
  var_dump( chunk_split( $str, $chunklen, $value) );
  $count++;
};
 
echo "Done"
?>
--EXPECTF--
*** Testing chunk_split() : different heredoc strings for 'ending' argument ***
-- Iteration 1 --
string(102) "This is str to check with heredoc ending.This	contains,
speci@! ch@r$ __with wrong \k escape char 222."
-- Iteration 2 --
string(102) "This is str to check with heredoc ending.This	contains,
speci@! ch@r$ __with wrong \k escape char 222."
-- Iteration 3 --
string(113) "This is star to checka with hereadoc endinga.This	contaains,
specai@! ch@r$ a__with wroang \k escaape char 22a2.a"
-- Iteration 4 --
string(421) "This is stThis is simple heredoc stringr to checkThis is simple heredoc string with hereThis is simple heredoc stringdoc endingThis is simple heredoc string.This	contThis is simple heredoc stringains,
specThis is simple heredoc stringi@! ch@r$ This is simple heredoc string__with wroThis is simple heredoc stringng \k escaThis is simple heredoc stringpe char 22This is simple heredoc string2.This is simple heredoc string"
-- Iteration 5 --
string(762) "This is stThis is to check chunk_split
function with multiline
heredocr to checkThis is to check chunk_split
function with multiline
heredoc with hereThis is to check chunk_split
function with multiline
heredocdoc endingThis is to check chunk_split
function with multiline
heredoc.This	contThis is to check chunk_split
function with multiline
heredocains,
specThis is to check chunk_split
function with multiline
heredoci@! ch@r$ This is to check chunk_split
function with multiline
heredoc__with wroThis is to check chunk_split
function with multiline
heredocng \k escaThis is to check chunk_split
function with multiline
heredocpe char 22This is to check chunk_split
function with multiline
heredoc2.This is to check chunk_split
function with multiline
heredoc"
-- Iteration 6 --
string(443) "This is stThis checks with $, %, &, charsr to checkThis checks with $, %, &, chars with hereThis checks with $, %, &, charsdoc endingThis checks with $, %, &, chars.This	contThis checks with $, %, &, charsains,
specThis checks with $, %, &, charsi@! ch@r$ This checks with $, %, &, chars__with wroThis checks with $, %, &, charsng \k escaThis checks with $, %, &, charspe char 22This checks with $, %, &, chars2.This checks with $, %, &, chars"
-- Iteration 7 --
string(487) "This is stThis checks	 and 
white space charsr to checkThis checks	 and 
white space chars with hereThis checks	 and 
white space charsdoc endingThis checks	 and 
white space chars.This	contThis checks	 and 
white space charsains,
specThis checks	 and 
white space charsi@! ch@r$ This checks	 and 
white space chars__with wroThis checks	 and 
white space charsng \k escaThis checks	 and 
white space charspe char 22This checks	 and 
white space chars2.This checks	 and 
white space chars"
-- Iteration 8 --
string(597) "This is st"To check " in heredoc".I'm sure it'll \work!r to check"To check " in heredoc".I'm sure it'll \work! with here"To check " in heredoc".I'm sure it'll \work!doc ending"To check " in heredoc".I'm sure it'll \work!.This	cont"To check " in heredoc".I'm sure it'll \work!ains,
spec"To check " in heredoc".I'm sure it'll \work!i@! ch@r$ "To check " in heredoc".I'm sure it'll \work!__with wro"To check " in heredoc".I'm sure it'll \work!ng \k esca"To check " in heredoc".I'm sure it'll \work!pe char 22"To check " in heredoc".I'm sure it'll \work!2."To check " in heredoc".I'm sure it'll \work!"
Done
