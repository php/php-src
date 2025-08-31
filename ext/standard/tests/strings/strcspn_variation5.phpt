--TEST--
Test strcspn() function : usage variations - with heredoc strings with default start and len args
--FILE--
<?php
/* Prototype  : proto int strcspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters not found in mask.
                If start or/and length is provided works like strcspn(substr($s,$start,$len),$bad_chars)
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strcspn() : with different heredoc strings as str argument
*/

echo "*** Testing strcspn() : with heredoc strings ***\n";

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

$mask = "fth12\ne67890\0\xaa\100o";


// loop through each element of the array for str argument

foreach($heredoc_strings as $str) {
      echo "\n-- Iteration with str value as \"$str\" --\n";
      var_dump( strcspn($str,$mask) ); // with default start and len values
};

echo "Done"
?>
--EXPECTF--
*** Testing strcspn() : with heredoc strings ***

-- Iteration with str value as "" --
int(0)

-- Iteration with str value as "

" --
int(0)

-- Iteration with str value as "first line of heredoc string
second line of heredoc string
third line of heredocstring" --
int(0)

-- Iteration with str value as "hello	world
hello
world
" --
int(0)

-- Iteration with str value as "hello123world456
1234hello	1234" --
int(0)

-- Iteration with str value as "hello%0world%0hello
%0hello%0" --
int(0)

-- Iteration with str value as "hello%0@ªworld%0hello
%0hello%0" --
int(0)
Done
