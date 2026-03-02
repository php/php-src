--TEST--
Test strcspn() function : usage variations - different strings with default start and len args
--FILE--
<?php
/* Prototype  : proto int strcspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters not found in mask.
                If start or/and length is provided works like strcspn(substr($s,$start,$len),$bad_chars)
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strcspn() : with different strings as str argument and default start and len args
*/

echo "*** Testing strcspn() : with different str and default start and len args ***\n";

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

$mask = "sft\n34lw56r78d90\0\xaa\100o";


// loop through each element of the array for str argument

foreach($strings as $str) {
      echo "\n-- Iteration with str value \"$str\" --\n";

      //calling strcspn() with default arguments
      var_dump( strcspn($str,$mask) );
};

echo "Done"
?>
--EXPECTF--
*** Testing strcspn() : with different str and default start and len args ***

-- Iteration with str value "" --
int(0)

-- Iteration with str value "" --
int(0)

-- Iteration with str value "
" --
int(0)

-- Iteration with str value "\n" --
int(2)

-- Iteration with str value "hello	world
hello
world
" --
int(2)

-- Iteration with str value "hello\tworld\nhello\nworld\n" --
int(2)

-- Iteration with str value "1234hello45world	123" --
int(2)

-- Iteration with str value "1234hello45world\t123" --
int(2)

-- Iteration with str value "hello%0world
" --
int(2)

-- Iteration with str value "hello\0world\012" --
int(2)

-- Iteration with str value "%0%0" --
int(0)

-- Iteration with str value "%0hello%0world%0" --
int(0)

-- Iteration with str value "%0hello\0world%0" --
int(0)

-- Iteration with str value "hello%0world" --
int(2)

-- Iteration with str value "hello%0world" --
int(2)

-- Iteration with str value "hello%0@ªaworld" --
int(2)

-- Iteration with str value "hello\0\100\xaaaworld" --
int(2)
Done
