--TEST--
Bug #61780 (Inconsistent PCRE captures in match results): numeric subpatterns
--FILE--
<?php
preg_match('/(4)?(2)?\d/', '23456', $matches, PREG_UNMATCHED_AS_NULL);
var_export($matches);
echo "\n\n";
preg_match('/(4)?(2)?\d/', '23456', $matches, PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_export($matches);
echo "\n\n";
preg_match_all('/(4)?(2)?\d/', '123456', $matches, PREG_UNMATCHED_AS_NULL);
var_export($matches);
echo "\n\n";
preg_match_all('/(4)?(2)?\d/', '123456', $matches, PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_export($matches);
echo "\n\n";
preg_match_all('/(4)?(2)?\d/', '123456', $matches, PREG_SET_ORDER | PREG_UNMATCHED_AS_NULL);
var_export($matches);
echo "\n\n";
preg_match_all('/(4)?(2)?\d/', '123456', $matches, PREG_SET_ORDER | PREG_OFFSET_CAPTURE | PREG_UNMATCHED_AS_NULL);
var_export($matches);
?>
--EXPECT--
array (
  0 => '23',
  1 => NULL,
  2 => '2',
)

array (
  0 => 
  array (
    0 => '23',
    1 => 0,
  ),
  1 => 
  array (
    0 => NULL,
    1 => -1,
  ),
  2 => 
  array (
    0 => '2',
    1 => 0,
  ),
)

array (
  0 => 
  array (
    0 => '1',
    1 => '23',
    2 => '45',
    3 => '6',
  ),
  1 => 
  array (
    0 => NULL,
    1 => NULL,
    2 => '4',
    3 => NULL,
  ),
  2 => 
  array (
    0 => NULL,
    1 => '2',
    2 => NULL,
    3 => NULL,
  ),
)

array (
  0 => 
  array (
    0 => 
    array (
      0 => '1',
      1 => 0,
    ),
    1 => 
    array (
      0 => '23',
      1 => 1,
    ),
    2 => 
    array (
      0 => '45',
      1 => 3,
    ),
    3 => 
    array (
      0 => '6',
      1 => 5,
    ),
  ),
  1 => 
  array (
    0 => NULL,
    1 => 
    array (
      0 => NULL,
      1 => -1,
    ),
    2 => 
    array (
      0 => '4',
      1 => 3,
    ),
    3 => NULL,
  ),
  2 => 
  array (
    0 => NULL,
    1 => 
    array (
      0 => '2',
      1 => 1,
    ),
    2 => NULL,
    3 => NULL,
  ),
)

array (
  0 => 
  array (
    0 => '1',
  ),
  1 => 
  array (
    0 => '23',
    1 => NULL,
    2 => '2',
  ),
  2 => 
  array (
    0 => '45',
    1 => '4',
  ),
  3 => 
  array (
    0 => '6',
  ),
)

array (
  0 => 
  array (
    0 => 
    array (
      0 => '1',
      1 => 0,
    ),
  ),
  1 => 
  array (
    0 => 
    array (
      0 => '23',
      1 => 1,
    ),
    1 => 
    array (
      0 => NULL,
      1 => -1,
    ),
    2 => 
    array (
      0 => '2',
      1 => 1,
    ),
  ),
  2 => 
  array (
    0 => 
    array (
      0 => '45',
      1 => 3,
    ),
    1 => 
    array (
      0 => '4',
      1 => 3,
    ),
  ),
  3 => 
  array (
    0 => 
    array (
      0 => '6',
      1 => 5,
    ),
  ),
)
