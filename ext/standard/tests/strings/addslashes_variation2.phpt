--TEST--
Test addslashes() function : usage variations - strings with characters to be backslashed
--FILE--
<?php
/* Prototype  : string addslashes ( string $str )
 * Description: Returns a string with backslashes before characters that need to be quoted in database queries etc.
 * Source code: ext/standard/string.c
*/

/*
 * Test addslashes() with various strings containing characters thats can be backslashed
*/

echo "*** Testing addslashes() : with various strings containing characters to be backslashed ***\n";

// initialising a heredoc string
$heredoc_string = <<<EOT
This is line 1 of 'heredoc' string
This is line 2 of "heredoc" string
EOT;

$heredoc_null_string =<<<EOT
EOT;

// initialising the string array

$str_array = array(
                    // string without any characters that can be backslashed
                    'Hello world',

                    // string with single quotes
                    "how're you doing?",
                    "don't disturb u'r neighbours",
                    "don't disturb u'r neighbours''",
                    '',
                    '\'',
                    "'",

                    // string with double quotes
                    'he said, "he will be on leave"',
                    'he said, ""he will be on leave"',
                    '"""PHP"""',
                    "",
                    "\"",
                    '"',
 		    "hello\"",

                    // string with backslash characters
                    'Is your name Ram\Krishna?',
                    '\\0.0.0.0',
                    'c:\php\testcase\addslashes',
                    '\\',

                    // string with nul characters
                    'hello'.chr(0).'world',
                    chr(0).'hello'.chr(0),
                    chr(0).chr(0).'hello',
                    chr(0),

                    // mixed strings
                    "'\\0.0.0.0'",
                    "'\\0.0.0.0'".chr(0),
                    chr(0)."'c:\php\'",
                    '"\\0.0.0.0"',
                    '"c:\php\"'.chr(0)."'",
                    '"hello"'."'world'".chr(0).'//',

		    // string with hexadecimal number
                    "0xABCDEF0123456789",
                    "\x00",
                    '!@#$%&*@$%#&/;:,<>',
                    "hello\x00world",

                    // heredoc strings
                    $heredoc_string,
                    $heredoc_null_string
                  );

$count = 1;
// looping to test for all strings in $str_array
foreach( $str_array as $str )  {
  echo "\n-- Iteration $count --\n";
  var_dump( addslashes($str) );
  $count ++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing addslashes() : with various strings containing characters to be backslashed ***

-- Iteration 1 --
string(11) "Hello world"

-- Iteration 2 --
string(18) "how\'re you doing?"

-- Iteration 3 --
string(30) "don\'t disturb u\'r neighbours"

-- Iteration 4 --
string(34) "don\'t disturb u\'r neighbours\'\'"

-- Iteration 5 --
string(0) ""

-- Iteration 6 --
string(2) "\'"

-- Iteration 7 --
string(2) "\'"

-- Iteration 8 --
string(32) "he said, \"he will be on leave\""

-- Iteration 9 --
string(34) "he said, \"\"he will be on leave\""

-- Iteration 10 --
string(15) "\"\"\"PHP\"\"\""

-- Iteration 11 --
string(0) ""

-- Iteration 12 --
string(2) "\""

-- Iteration 13 --
string(2) "\""

-- Iteration 14 --
string(7) "hello\""

-- Iteration 15 --
string(26) "Is your name Ram\\Krishna?"

-- Iteration 16 --
string(9) "\\0.0.0.0"

-- Iteration 17 --
string(29) "c:\\php\\testcase\\addslashes"

-- Iteration 18 --
string(2) "\\"

-- Iteration 19 --
string(12) "hello\0world"

-- Iteration 20 --
string(9) "\0hello\0"

-- Iteration 21 --
string(9) "\0\0hello"

-- Iteration 22 --
string(2) "\0"

-- Iteration 23 --
string(13) "\'\\0.0.0.0\'"

-- Iteration 24 --
string(15) "\'\\0.0.0.0\'\0"

-- Iteration 25 --
string(15) "\0\'c:\\php\\\'"

-- Iteration 26 --
string(13) "\"\\0.0.0.0\""

-- Iteration 27 --
string(17) "\"c:\\php\\\"\0\'"

-- Iteration 28 --
string(22) "\"hello\"\'world\'\0//"

-- Iteration 29 --
string(18) "0xABCDEF0123456789"

-- Iteration 30 --
string(2) "\0"

-- Iteration 31 --
string(18) "!@#$%&*@$%#&/;:,<>"

-- Iteration 32 --
string(12) "hello\0world"

-- Iteration 33 --
string(7%d) "This is line 1 of \'heredoc\' string
This is line 2 of \"heredoc\" string"

-- Iteration 34 --
string(0) ""
Done
