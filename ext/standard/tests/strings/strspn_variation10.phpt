--TEST--
Test strspn() function : usage variations - with varying mask & default start and len args
--FILE--
<?php
/*
* Testing strspn() : with varying mask and default start and len arguments
*/

echo "*** Testing strspn() : with different mask strings and default start and len arguments ***\n";

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

// define the array of mask strings
$mask_array = array(
            "",
            '',
            "f\n\trelshti \l",
            'f\n\trelsthi \l',
            "\telh",
            "t\ ",
            '\telh',
            "felh\t\ ",
            " \t",
                    "fhel\t\i\100\xa"
                   );


// loop through each element of the array for mask argument

$count = 1;
foreach($strings as $str)  {
  echo "\n-- Iteration $count --\n";
  foreach($mask_array as $mask)  {
      var_dump( strspn($str,$mask) );
  }
  $count++;
}

echo "Done"
?>
--EXPECT--
*** Testing strspn() : with different mask strings and default start and len arguments ***

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
int(0)
int(0)
int(1)
int(0)
int(0)
int(0)
int(0)
int(0)
int(0)
int(1)

-- Iteration 4 --
int(0)
int(0)
int(1)
int(2)
int(0)
int(1)
int(1)
int(1)
int(0)
int(1)

-- Iteration 5 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 6 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 7 --
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

-- Iteration 8 --
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

-- Iteration 9 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 10 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 11 --
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

-- Iteration 12 --
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

-- Iteration 13 --
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

-- Iteration 14 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 15 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 16 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)

-- Iteration 17 --
int(0)
int(0)
int(4)
int(4)
int(4)
int(0)
int(4)
int(4)
int(0)
int(4)
Done
