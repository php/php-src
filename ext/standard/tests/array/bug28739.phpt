--TEST--
Bug #28739 (*diff() and *intersect() not clearing the fci cache before work)
--FILE--
<?php
class p {
   public $x;
   function __construct($x){$this->x=$x;}
}
function a(&$a, &$b){var_dump(__FUNCTION__);return $a->x - $b->x;}
function b(&$a, &$b){var_dump(__FUNCTION__);return $a->x - $b->x;}

$p1 = array(new p(2), new p(1), new p(0));
$p2 = array(new p(0), new p(2), new p(3));

uasort($p1, 'a');
print_r($p1);
echo "Now diffing:\n";
print_r(array_udiff($p1,$p2, 'b'));
?>
--EXPECT--
string(1) "a"
string(1) "a"
Array
(
    [2] => p Object
        (
            [x] => 0
        )

    [1] => p Object
        (
            [x] => 1
        )

    [0] => p Object
        (
            [x] => 2
        )

)
Now diffing:
string(1) "b"
string(1) "b"
string(1) "b"
string(1) "b"
string(1) "b"
string(1) "b"
string(1) "b"
string(1) "b"
string(1) "b"
Array
(
    [1] => p Object
        (
            [x] => 1
        )

)