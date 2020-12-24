--TEST--
SplFixedArray: __set_state
--FILE--
<?php

$fixed = SplFixedArray::__set_state([1, 2, 3]);
var_export($fixed);
echo "\n---\n";

// dynamic properties
$fixed->undefined = 'undef';
var_export($fixed);
echo "\n---\n";

// inheritance
class Vec
    extends SplFixedArray
{
    public $type;
}
$vec = new Vec(3);
$vec[0] = 1;
$vec[1] = 2;
$vec[2] = 3;
$vec->type = 'int';
var_export($vec);
echo "\n---\n";

$vec->undefined = 'undef';
var_export($vec);

?>
--EXPECT--
SplFixedArray::__set_state(array(
   0 => 1,
   1 => 2,
   2 => 3,
))
---
SplFixedArray::__set_state(array(
   'undefined' => 'undef',
   0 => 1,
   1 => 2,
   2 => 3,
))
---
Vec::__set_state(array(
   'type' => 'int',
   0 => 1,
   1 => 2,
   2 => 3,
))
---
Vec::__set_state(array(
   'type' => 'int',
   'undefined' => 'undef',
   0 => 1,
   1 => 2,
   2 => 3,
))
