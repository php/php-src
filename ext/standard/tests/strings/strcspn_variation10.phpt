--TEST--
Test strcspn() function : usage variations - with varying mask & default start and len args
--FILE--
<?php
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
  echo "\n-- Itearation $count --\n";
  foreach($mask_array as $mask) {
      var_dump( strcspn($str,$mask) );
  }
  $count++;
}

echo "Done"
?>
--EXPECT--
*** Testing strcspn() : with different mask strings and default start and len arguments ***

-- Itearation 1 --
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

-- Itearation 2 --
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

-- Itearation 3 --
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

-- Itearation 4 --
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

-- Itearation 5 --
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

-- Itearation 6 --
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

-- Itearation 7 --
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

-- Itearation 8 --
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

-- Itearation 9 --
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

-- Itearation 10 --
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

-- Itearation 11 --
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

-- Itearation 12 --
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

-- Itearation 13 --
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

-- Itearation 14 --
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

-- Itearation 15 --
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

-- Itearation 16 --
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

-- Itearation 17 --
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
