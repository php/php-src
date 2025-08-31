--TEST--
Test strcspn() function : usage variations - with heredoc strings, varying mask & default start and len args
--FILE--
<?php
/*
* Testing strcspn() : with heredoc string, varying mask and default start and len arguments
*/

echo "*** Testing strcspn() : with different mask strings ***\n";

// initialing required variables
// defining different heredoc strings
$empty_heredoc = <<<EOT
EOT;

$heredoc_with_newline = <<<EOT
\n

EOT;

$heredoc_with_characters = <<<EOT
first line of heredoc string
second line of heredoc string
third line of heredocstring
EOT;

$heredoc_with_newline_and_tabs = <<<EOT
hello\tworld\nhello\nworld\n
EOT;

$heredoc_with_alphanumerics = <<<EOT
hello123world456
1234hello\t1234
EOT;

$heredoc_with_embedded_nulls = <<<EOT
hello\0world\0hello
\0hello\0
EOT;

$heredoc_with_hexa_octal = <<<EOT
hello\0\100\xaaworld\0hello
\0hello\0
EOT;

$heredoc_strings = array(
                   $empty_heredoc,
                   $heredoc_with_newline,
                   $heredoc_with_characters,
                   $heredoc_with_newline_and_tabs,
                   $heredoc_with_alphanumerics,
                   $heredoc_with_embedded_nulls,
                   $heredoc_with_hexa_octal
                   );

// defining array of mask strings
$mask_array = array(
            "",
            '',
            "\n\trsti \l",
            '\n\trsti \l',
            "\t",
            "t\ ",
            '\t',
            "\t\ ",
            " \t",
                    "\t\i\100\xaa"
                   );


// loop through each element of the arrays for string and mask arguments

$count = 1;
foreach($heredoc_strings as $str) {
  echo "\n-- Iteration $count --\n";
  foreach($mask_array as $mask) {
      var_dump( strcspn($str,$mask) ); // with default start and len value
  }
  $count++;
}

echo "Done"
?>
--EXPECT--
*** Testing strcspn() : with different mask strings ***

-- Iteration 1 --
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)

-- Iteration 2 --
int(2)
int(2)
int(0)
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)

-- Iteration 3 --
int(86)
int(86)
int(1)
int(1)
int(86)
int(4)
int(4)
int(5)
int(5)
int(1)

-- Iteration 4 --
int(24)
int(24)
int(2)
int(2)
int(5)
int(24)
int(24)
int(5)
int(5)
int(5)

-- Iteration 5 --
int(31)
int(31)
int(2)
int(2)
int(26)
int(31)
int(31)
int(26)
int(26)
int(26)

-- Iteration 6 --
int(25)
int(25)
int(2)
int(2)
int(25)
int(25)
int(25)
int(25)
int(25)
int(25)

-- Iteration 7 --
int(27)
int(27)
int(2)
int(2)
int(27)
int(27)
int(27)
int(27)
int(27)
int(6)
Done
