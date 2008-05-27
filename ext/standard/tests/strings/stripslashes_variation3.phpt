--TEST--
Test stripslashes() function : usage variations - strings with newline and tab characters
--FILE--
<?php
/* Prototype  : string stripslashes ( string $str )
 * Description: Returns an un-quoted string
 * Source code: ext/standard/string.c
*/

/*
 * Test stripslashes() with strings containing newline and tab characters.
*/

echo "*** Testing stripslashes() : with strings containing newline and tab characters ***\n";

// initialising  heredoc strings
$heredoc_string_with_newline = <<<EOT
This is line 1 \nof 'heredoc' string
This is line 2 \nof "heredoc" string
EOT;

$heredoc_string_with_tab = <<<EOT
This is line 1 \tof 'heredoc' string
This is line 2 \tof "heredoc" string
EOT;
// initialising the string array

$str_array = array( 
                    // string with newline character
                    "\n",
		    "\\n",
                    "Hello \nworld",
                    "Hello \\nworld",
                    '\n',
		    '\\n',
                    'Hello \nworld',
                    'Hello \\nworld',
                    $heredoc_string_with_newline,
 
                    // string with tab character
 		    "\t",
		    "\\t",
                    "Hello \tworld",
                    "Hello \\tworld",
 		    '\t',
		    '\\t',
                    'Hello \tworld',
                    'Hello \\tworld',
                    $heredoc_string_with_tab
                  );

$count = 1;
// looping to test for all strings in $str_array
foreach( $str_array as $str )  {
  echo "\n-- Iteration $count --\n";
  var_dump( stripslashes($str) );
  $count ++;
}

echo "Done\n";
?>
--EXPECT--
*** Testing stripslashes() : with strings containing newline and tab characters ***

-- Iteration 1 --
unicode(1) "
"

-- Iteration 2 --
unicode(1) "n"

-- Iteration 3 --
unicode(12) "Hello 
world"

-- Iteration 4 --
unicode(12) "Hello nworld"

-- Iteration 5 --
unicode(1) "n"

-- Iteration 6 --
unicode(1) "n"

-- Iteration 7 --
unicode(12) "Hello nworld"

-- Iteration 8 --
unicode(12) "Hello nworld"

-- Iteration 9 --
unicode(71) "This is line 1 
of 'heredoc' string
This is line 2 
of "heredoc" string"

-- Iteration 10 --
unicode(1) "	"

-- Iteration 11 --
unicode(1) "t"

-- Iteration 12 --
unicode(12) "Hello 	world"

-- Iteration 13 --
unicode(12) "Hello tworld"

-- Iteration 14 --
unicode(1) "t"

-- Iteration 15 --
unicode(1) "t"

-- Iteration 16 --
unicode(12) "Hello tworld"

-- Iteration 17 --
unicode(12) "Hello tworld"

-- Iteration 18 --
unicode(71) "This is line 1 	of 'heredoc' string
This is line 2 	of "heredoc" string"
Done
