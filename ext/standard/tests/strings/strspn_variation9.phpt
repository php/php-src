--TEST--
Test strspn() function : usage variations - different strings with default start and len args
--FILE--
<?php
/* Prototype  : proto int strspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters found in mask.
                If start or/and length is provided works like strspn(substr($s,$start,$len),$good_chars)
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strspn() : with different strings as str argument and default start and len args
*/

echo "*** Testing strspn() : with different str and default start and len args ***\n";

// initialing required variables
// defining different strings

$strings = array(
                   "",
           '',
           "\n",
           '\n',
           "hello\tworld\nhello\nworld\n",
           'hello\tworld\nhello\nworld\n',
           "1234hello45world\t123",
           '1234hello45world\t123',
           "hello\0world\012",
           'hello\0world\012',
           chr(0).chr(0),
           chr(0)."hello\0world".chr(0),
           chr(0).'hello\0world'.chr(0),
           "hello".chr(0)."world",
           'hello'.chr(0).'world',
           "hello\0\100\xaaaworld",
           'hello\0\100\xaaaworld'
                   );

$mask = "sfth12\ne34lw56r78d90\0\xaa\100o";


// loop through each element of the array for str argument

foreach($strings as $str) {
      echo "\n-- Iteration with str value \"$str\" --\n";

      //calling strspn() with default arguments
      var_dump( strspn($str,$mask) );
};

echo "Done"
?>
--EXPECTF--
*** Testing strspn() : with different str and default start and len args ***

-- Iteration with str value "" --
int(0)

-- Iteration with str value "" --
int(0)

-- Iteration with str value "
" --
int(1)

-- Iteration with str value "\n" --
int(0)

-- Iteration with str value "hello	world
hello
world
" --
int(5)

-- Iteration with str value "hello\tworld\nhello\nworld\n" --
int(5)

-- Iteration with str value "1234hello45world	123" --
int(16)

-- Iteration with str value "1234hello45world\t123" --
int(16)

-- Iteration with str value "hello%0world
" --
int(12)

-- Iteration with str value "hello\0world\012" --
int(5)

-- Iteration with str value "%0%0" --
int(2)

-- Iteration with str value "%0hello%0world%0" --
int(13)

-- Iteration with str value "%0hello\0world%0" --
int(6)

-- Iteration with str value "hello%0world" --
int(11)

-- Iteration with str value "hello%0world" --
int(11)

-- Iteration with str value "hello%0@ªaworld" --
int(8)

-- Iteration with str value "hello\0\100\xaaaworld" --
int(5)
Done
