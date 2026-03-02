--TEST--
Test crc32() function : usage variations - heredoc strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4)
  die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/*
 * Testing crc32() : with different heredoc strings passed to the str argument
*/

echo "*** Testing crc32() : with different heredoc strings ***\n";

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

$heredoc_with_long_string = <<<EOT
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbb
cccccccccccccccccccccccccccccccccddddddddddddddddddddddddddddddddd
eeeeeeeeeeeeeeeeeeeeeeeeeeeeffffffffffffffffffffffffffffffffffffff
gggggggggggggggggggggggggggggggggggggggggghhhhhhhhhhhhhhhhhhhhhhhh
111111111111111111111122222222222222222222222222222222222222222222
333333333333333333333333333333333334444444444444444444444444444444
555555555555555555555555555555555555555555556666666666666666666666
777777777777777777777777777777777777777777777777777777777777777777
/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t
/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n
EOT;

$heredoc_strings = array(
                   $empty_heredoc,
                   $heredoc_with_newline,
                   $heredoc_with_characters,
                   $heredoc_with_newline_and_tabs,
                   $heredoc_with_alphanumerics,
                   $heredoc_with_embedded_nulls,
                   $heredoc_with_hexa_octal,
           $heredoc_with_long_string
                   );

// loop to test the function with each heredoc string in the array

$count = 1;
foreach($heredoc_strings as $str) {
  echo "\n-- Iteration $count --\n";
  var_dump( crc32($str) );
  $count++;
}

echo "Done";
?>
--EXPECT--
*** Testing crc32() : with different heredoc strings ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(1541608299)

-- Iteration 3 --
int(1588851550)

-- Iteration 4 --
int(-1726108239)

-- Iteration 5 --
int(-1847303891)

-- Iteration 6 --
int(-1260053120)

-- Iteration 7 --
int(-1718044186)

-- Iteration 8 --
int(1646793751)
Done
