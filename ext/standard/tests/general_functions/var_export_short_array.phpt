--TEST--
Test var_export() function with short array option
--FILE--
<?php
    $x = new stdClass();
    $x->{'\'\\'} = 7;
    echo var_export($x, false, true)."\n";

    var_export([[1, 2], ['a', 'b']], false, true);
?>
--EXPECT--
(object) [
   '\'\\' => 7,
]
[
  0 => [
    0 => 1,
    1 => 2,
  ],
  1 => [
    0 => 'a',
    1 => 'b',
  ],
]
