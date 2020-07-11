--TEST--
Test crc32() function : usage variations - double quoted strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4)
  die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
/*
 * Testing crc32() : with different strings in double quotes passed to the function
*/

echo "*** Testing crc32() : with different strings in double quotes ***\n";

// defining an array of strings
$string_array = array(
  "",
  " ",
  "hello world",
  "HELLO WORLD",
  " helloworld ",

  "(hello world)",
  "hello(world)",
  "helloworld()",
  "hello()(world",

  "'hello' world",
  "hello 'world'",
  "hello''world",

  "hello\tworld",
  "hellowor\\tld",
  "\thello world\t",
  "helloworld",
  "hellowor\\ld",
  "hello\nworld",
  "hellowor\\nld",
  "\nhello world\n",
  "\n\thelloworld",
  "hel\tlo\n world",

  "!@#$%&",
  "#hello@world.com",
  "$hello$world",

  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbb
   cccccccccccccccccccccccccccccccccddddddddddddddddddddddddddddddddd
   eeeeeeeeeeeeeeeeeeeeeeeeeeeeffffffffffffffffffffffffffffffffffffff
   gggggggggggggggggggggggggggggggggggggggggghhhhhhhhhhhhhhhhhhhhhhhh
   111111111111111111111122222222222222222222222222222222222222222222
   333333333333333333333333333333333334444444444444444444444444444444
   555555555555555555555555555555555555555555556666666666666666666666
   777777777777777777777777777777777777777777777777777777777777777777
   /t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t/t
   /n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n/n"
);

// looping to check the behaviour of the function for each string in the array

$count = 1;
foreach($string_array as $str) {
  echo "\n-- Iteration $count --\n";
  var_dump( crc32($str) );
  $count++;
}

echo "Done";
?>
--EXPECTF--
*** Testing crc32() : with different strings in double quotes ***

Warning: Undefined variable $hello in %s on line %d

Warning: Undefined variable $world in %s on line %d

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(-378745019)

-- Iteration 3 --
int(222957957)

-- Iteration 4 --
int(-2015000997)

-- Iteration 5 --
int(1234261835)

-- Iteration 6 --
int(-1867296214)

-- Iteration 7 --
int(1048577080)

-- Iteration 8 --
int(2129739710)

-- Iteration 9 --
int(-1633247628)

-- Iteration 10 --
int(1191242624)

-- Iteration 11 --
int(603128807)

-- Iteration 12 --
int(-525789576)

-- Iteration 13 --
int(770262395)

-- Iteration 14 --
int(243585859)

-- Iteration 15 --
int(-986324846)

-- Iteration 16 --
int(-102031187)

-- Iteration 17 --
int(-588181215)

-- Iteration 18 --
int(-1417857067)

-- Iteration 19 --
int(523630053)

-- Iteration 20 --
int(-503915034)

-- Iteration 21 --
int(-254912432)

-- Iteration 22 --
int(-1581578467)

-- Iteration 23 --
int(-1828940657)

-- Iteration 24 --
int(-1654468652)

-- Iteration 25 --
int(0)

-- Iteration 26 --
int(1431761713)
Done
