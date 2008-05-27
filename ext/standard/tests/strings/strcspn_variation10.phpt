--TEST--
Test strcspn() function : usage variations - with varying mask & default start and len args (Bug #42731)
--FILE--
<?php
/* Prototype  : proto int strcspn(string str, string mask [, int start [, int len]])
 * Description: Finds length of initial segment consisting entirely of characters not found in mask.
                If start or/and length is provided works like strcspn(substr($s,$start,$len),$bad_chars) 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Testing strcspn() : with varying mask and default start and len arguments
*/

echo "*** Testing strcspn() : with different mask strings and default start and len arguments ***\n";

// initialing required variables
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
                    "\t\i\100\xa"
                   );
		

// loop through each element of the array for mask argument
$count = 1;
foreach($strings as $str) {
  echo "\n-- Iteration $count --\n";
  foreach($mask_array as $mask) {
      var_dump( strcspn($str,$mask) );
  }
  $count++;
}

echo "Done"
?>
--EXPECT--
*** Testing strcspn() : with different mask strings and default start and len arguments ***

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
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)

-- Iteration 3 --
int(1)
int(1)
int(0)
int(1)
int(1)
int(1)
int(1)
int(1)
int(1)
int(0)

-- Iteration 4 --
int(2)
int(2)
int(0)
int(0)
int(2)
int(0)
int(0)
int(0)
int(2)
int(0)

-- Iteration 5 --
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

-- Iteration 6 --
int(28)
int(28)
int(2)
int(2)
int(28)
int(5)
int(5)
int(5)
int(28)
int(5)

-- Iteration 7 --
int(20)
int(20)
int(6)
int(6)
int(16)
int(20)
int(20)
int(16)
int(16)
int(16)

-- Iteration 8 --
int(21)
int(21)
int(6)
int(6)
int(21)
int(16)
int(16)
int(16)
int(21)
int(16)

-- Iteration 9 --
int(5)
int(5)
int(2)
int(2)
int(12)
int(12)
int(12)
int(12)
int(12)
int(11)

-- Iteration 10 --
int(16)
int(16)
int(2)
int(2)
int(16)
int(5)
int(5)
int(5)
int(16)
int(5)

-- Iteration 11 --
int(0)
int(0)
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)

-- Iteration 12 --
int(0)
int(0)
int(3)
int(3)
int(13)
int(13)
int(13)
int(13)
int(13)
int(13)

-- Iteration 13 --
int(0)
int(0)
int(3)
int(3)
int(14)
int(6)
int(6)
int(6)
int(14)
int(6)

-- Iteration 14 --
int(5)
int(5)
int(2)
int(2)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)

-- Iteration 15 --
int(5)
int(5)
int(2)
int(2)
int(11)
int(11)
int(11)
int(11)
int(11)
int(11)

-- Iteration 16 --
int(5)
int(5)
int(2)
int(2)
int(14)
int(14)
int(14)
int(14)
int(14)
int(6)

-- Iteration 17 --
int(21)
int(21)
int(2)
int(2)
int(21)
int(5)
int(5)
int(5)
int(21)
int(5)
Done
