--TEST--
Test strspn() function : usage variations - with heredoc strings, varying mask & default start and len args
--FILE--
<?php
/* Prototype  : proto int strspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters found in mask.
                If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars)
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strspn() : with heredoc string, varying mask and default start and len arguments
*/

echo "*** Testing strspn() : with different mask strings ***\n";

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

// defining array of different heredoc strings
$heredoc_strings = array(
                   $empty_heredoc,
                   $heredoc_with_newline,
                   $heredoc_with_characters,
                   $heredoc_with_newline_and_tabs,
                   $heredoc_with_alphanumerics,
                   $heredoc_with_embedded_nulls,
                   $heredoc_with_hexa_octal
                   );

// defining array of different mask strings
$mask_array = array(
		    "",
		    '',
		    "fh\ne\trlsti \l",
		    'fieh\n\trlsti \l',
		    "\t",
		    "lt\ ",
		    'l\t',
		    "fl\t\eh ",
		    "l \te",
                    "lf\the\i\100\xaa"
                   );


// loop through each element of the array for different heredoc and mask strings

$count = 1;

foreach($heredoc_strings as $str)  {
  echo "\n-- Iteration $count --\n";
  foreach($mask_array as $mask)  {
      var_dump( strspn($str,$mask) ); // with default start and len value
  }
  $count++;
}

echo "Done"
?>
--EXPECT--
*** Testing strspn() : with different mask strings ***

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
int(0)
int(0)
int(2)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)

-- Iteration 3 --
int(0)
int(0)
int(8)
int(11)
int(0)
int(0)
int(0)
int(1)
int(0)
int(2)

-- Iteration 4 --
int(0)
int(0)
int(4)
int(4)
int(0)
int(0)
int(0)
int(1)
int(0)
int(4)

-- Iteration 5 --
int(0)
int(0)
int(4)
int(4)
int(0)
int(0)
int(0)
int(1)
int(0)
int(4)

-- Iteration 6 --
int(0)
int(0)
int(4)
int(4)
int(0)
int(0)
int(0)
int(1)
int(0)
int(4)

-- Iteration 7 --
int(0)
int(0)
int(4)
int(4)
int(0)
int(0)
int(0)
int(1)
int(0)
int(4)
Done
