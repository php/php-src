--TEST--
Test sprintf() function : usage variations - string formats with string values
--FILE--
<?php
echo "*** Testing sprintf() : string formats with string values ***\n";

// defining different heredoc strings
/* string created using Heredoc (<<<) */
$heredoc_string = <<<EOT
This is string defined
using heredoc.
EOT;

/* heredoc string with only numerics */
$heredoc_numeric_string = <<<EOT
123456 3993
4849 string
EOT;

/* null heardoc string */
$heredoc_empty_string = <<<EOT
EOT;
$heredoc_null_string = <<<EOT
NULL
EOT;

// array of strings used to test the function
$string_values = array(
  "",
  " ",
  '',
  ' ',
  "string",
  'string',
  "NULL",
  'null',
  "FALSE",
  'true',
  "\x0b",
  "\0",
  '\0',
  '\060',
  "\070",
  "0x55F",
  "055",
  "@#$#$%%$^^$%^%^$^&",
  $heredoc_string,
  $heredoc_numeric_string,
  $heredoc_empty_string,
  $heredoc_null_string
);

// array of string formats
$string_formats = array(
  "%s", "%ls", " %s", "%s ",
  "\t%s", "\n%s", "%4s", "%30s",
);

$count = 1;
foreach($string_values as $string_value) {
  echo "\n-- Iteration $count --\n";

  foreach($string_formats as $format) {
    var_dump( sprintf($format, $string_value) );
  }
  $count++;
};

echo "Done";
?>
--EXPECTF--
*** Testing sprintf() : string formats with string values ***

-- Iteration 1 --
string(0) ""
string(0) ""
string(1) " "
string(1) " "
string(1) "	"
string(1) "
"
string(4) "    "
string(30) "                              "

-- Iteration 2 --
string(1) " "
string(1) " "
string(2) "  "
string(2) "  "
string(2) "	 "
string(2) "
 "
string(4) "    "
string(30) "                              "

-- Iteration 3 --
string(0) ""
string(0) ""
string(1) " "
string(1) " "
string(1) "	"
string(1) "
"
string(4) "    "
string(30) "                              "

-- Iteration 4 --
string(1) " "
string(1) " "
string(2) "  "
string(2) "  "
string(2) "	 "
string(2) "
 "
string(4) "    "
string(30) "                              "

-- Iteration 5 --
string(6) "string"
string(6) "string"
string(7) " string"
string(7) "string "
string(7) "	string"
string(7) "
string"
string(6) "string"
string(30) "                        string"

-- Iteration 6 --
string(6) "string"
string(6) "string"
string(7) " string"
string(7) "string "
string(7) "	string"
string(7) "
string"
string(6) "string"
string(30) "                        string"

-- Iteration 7 --
string(4) "NULL"
string(4) "NULL"
string(5) " NULL"
string(5) "NULL "
string(5) "	NULL"
string(5) "
NULL"
string(4) "NULL"
string(30) "                          NULL"

-- Iteration 8 --
string(4) "null"
string(4) "null"
string(5) " null"
string(5) "null "
string(5) "	null"
string(5) "
null"
string(4) "null"
string(30) "                          null"

-- Iteration 9 --
string(5) "FALSE"
string(5) "FALSE"
string(6) " FALSE"
string(6) "FALSE "
string(6) "	FALSE"
string(6) "
FALSE"
string(5) "FALSE"
string(30) "                         FALSE"

-- Iteration 10 --
string(4) "true"
string(4) "true"
string(5) " true"
string(5) "true "
string(5) "	true"
string(5) "
true"
string(4) "true"
string(30) "                          true"

-- Iteration 11 --
string(1) ""
string(1) ""
string(2) " "
string(2) " "
string(2) "	"
string(2) "
"
string(4) "   "
string(30) "                             "

-- Iteration 12 --
string(1) "%0"
string(1) "%0"
string(2) " %0"
string(2) "%0 "
string(2) "	%0"
string(2) "
%0"
string(4) "   %0"
string(30) "                             %0"

-- Iteration 13 --
string(2) "\0"
string(2) "\0"
string(3) " \0"
string(3) "\0 "
string(3) "	\0"
string(3) "
\0"
string(4) "  \0"
string(30) "                            \0"

-- Iteration 14 --
string(4) "\060"
string(4) "\060"
string(5) " \060"
string(5) "\060 "
string(5) "	\060"
string(5) "
\060"
string(4) "\060"
string(30) "                          \060"

-- Iteration 15 --
string(1) "8"
string(1) "8"
string(2) " 8"
string(2) "8 "
string(2) "	8"
string(2) "
8"
string(4) "   8"
string(30) "                             8"

-- Iteration 16 --
string(5) "0x55F"
string(5) "0x55F"
string(6) " 0x55F"
string(6) "0x55F "
string(6) "	0x55F"
string(6) "
0x55F"
string(5) "0x55F"
string(30) "                         0x55F"

-- Iteration 17 --
string(3) "055"
string(3) "055"
string(4) " 055"
string(4) "055 "
string(4) "	055"
string(4) "
055"
string(4) " 055"
string(30) "                           055"

-- Iteration 18 --
string(18) "@#$#$%%$^^$%^%^$^&"
string(18) "@#$#$%%$^^$%^%^$^&"
string(19) " @#$#$%%$^^$%^%^$^&"
string(19) "@#$#$%%$^^$%^%^$^& "
string(19) "	@#$#$%%$^^$%^%^$^&"
string(19) "
@#$#$%%$^^$%^%^$^&"
string(18) "@#$#$%%$^^$%^%^$^&"
string(30) "            @#$#$%%$^^$%^%^$^&"

-- Iteration 19 --
string(37) "This is string defined
using heredoc."
string(37) "This is string defined
using heredoc."
string(38) " This is string defined
using heredoc."
string(38) "This is string defined
using heredoc. "
string(38) "	This is string defined
using heredoc."
string(38) "
This is string defined
using heredoc."
string(37) "This is string defined
using heredoc."
string(37) "This is string defined
using heredoc."

-- Iteration 20 --
string(23) "123456 3993
4849 string"
string(23) "123456 3993
4849 string"
string(24) " 123456 3993
4849 string"
string(24) "123456 3993
4849 string "
string(24) "	123456 3993
4849 string"
string(24) "
123456 3993
4849 string"
string(23) "123456 3993
4849 string"
string(30) "       123456 3993
4849 string"

-- Iteration 21 --
string(0) ""
string(0) ""
string(1) " "
string(1) " "
string(1) "	"
string(1) "
"
string(4) "    "
string(30) "                              "

-- Iteration 22 --
string(4) "NULL"
string(4) "NULL"
string(5) " NULL"
string(5) "NULL "
string(5) "	NULL"
string(5) "
NULL"
string(4) "NULL"
string(30) "                          NULL"
Done
