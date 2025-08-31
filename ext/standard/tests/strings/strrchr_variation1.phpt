--TEST--
Test strrchr() function : usage variations - double quoted strings
--FILE--
<?php

/* Test strrchr() function by passing various double quoted strings for 'haystack' & 'needle' */

echo "*** Testing strrchr() function: with various double quoted strings ***";
$haystack = "Hello,\t\n\0\n  $&!#%\o,()*+-./:;<=>?@hello123456he \x234 \101 ";
$needle = array(
  //regular strings
  "l",
  "L",
  "HELLO",
  "hEllo",

  //escape characters
  "\t",
  "\T",
  "	",
  "\n",
  "\N",
  "
",  //new line

  //nulls
  "\0",

  //boolean false
  FALSE,
  false,

  //empty string
  "",

  //special chars
  " ",
  "$",
  " $",
  "&",
  "!#",
  "%\o",
  "\o,",
  "()",
  "*+",
  "+",
  "-",
  ".",
  ".;",
  ":;",
  ";",
  "<=>",
  ">",
  "=>",
  "?",
  "@",
  "@hEllo",

  "12345", //decimal numeric string
  "\x23",  //hexadecimal numeric string
  "#",  //respective ASCII char of \x23
  "\101",  //octal numeric string
  "A",  //respective ASCII char of \101
  "456HEE",  //numerics + chars
  42, //needle as int(ASCII value of "*")
  $haystack  //haystack as needle
);

/* loop through to get the position of the needle in haystack string */
$count = 1;
for($index=0; $index<count($needle); $index++) {
  echo "\n-- Iteration $count --\n";
  var_dump( strrchr($haystack, $needle[$index]) );
  $count++;
}
echo "*** Done ***";
?>
--EXPECTF--
*** Testing strrchr() function: with various double quoted strings ***
-- Iteration 1 --
string(16) "lo123456he #4 A "

-- Iteration 2 --
bool(false)

-- Iteration 3 --
string(53) "Hello,	
%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 4 --
string(8) "he #4 A "

-- Iteration 5 --
string(47) "	
%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 6 --
string(36) "\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 7 --
string(47) "	
%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 8 --
string(44) "
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 9 --
string(36) "\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 10 --
string(44) "
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 11 --
string(45) "%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 12 --
string(45) "%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 13 --
string(45) "%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 14 --
string(45) "%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 15 --
string(1) " "

-- Iteration 16 --
string(41) "$&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 17 --
string(1) " "

-- Iteration 18 --
string(40) "&!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 19 --
string(39) "!#%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 20 --
string(37) "%\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 21 --
string(36) "\o,()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 22 --
string(33) "()*+-./:;<=>?@hello123456he #4 A "

-- Iteration 23 --
string(31) "*+-./:;<=>?@hello123456he #4 A "

-- Iteration 24 --
string(30) "+-./:;<=>?@hello123456he #4 A "

-- Iteration 25 --
string(29) "-./:;<=>?@hello123456he #4 A "

-- Iteration 26 --
string(28) "./:;<=>?@hello123456he #4 A "

-- Iteration 27 --
string(28) "./:;<=>?@hello123456he #4 A "

-- Iteration 28 --
string(26) ":;<=>?@hello123456he #4 A "

-- Iteration 29 --
string(25) ";<=>?@hello123456he #4 A "

-- Iteration 30 --
string(24) "<=>?@hello123456he #4 A "

-- Iteration 31 --
string(22) ">?@hello123456he #4 A "

-- Iteration 32 --
string(23) "=>?@hello123456he #4 A "

-- Iteration 33 --
string(21) "?@hello123456he #4 A "

-- Iteration 34 --
string(20) "@hello123456he #4 A "

-- Iteration 35 --
string(20) "@hello123456he #4 A "

-- Iteration 36 --
string(14) "123456he #4 A "

-- Iteration 37 --
string(5) "#4 A "

-- Iteration 38 --
string(5) "#4 A "

-- Iteration 39 --
string(2) "A "

-- Iteration 40 --
string(2) "A "

-- Iteration 41 --
string(4) "4 A "

-- Iteration 42 --
string(4) "4 A "

-- Iteration 43 --
string(53) "Hello,	
%0
  $&!#%\o,()*+-./:;<=>?@hello123456he #4 A "
*** Done ***
