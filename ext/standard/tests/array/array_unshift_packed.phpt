--TEST--
array_unshift() on packed arrays
--FILE--
<?php
function dump(array $a): void {
    echo json_encode($a), ' key: ', var_export(key($a), true), "\n";
}

// The values fill the remaining slots of the 16-slot table exactly
$a = range(1, 14);
end($a);
var_dump(array_unshift($a, -1, 0));
dump($a);

// The 16-slot table has no slot left for the value
$a = range(1, 16);
end($a);
var_dump(array_unshift($a, 0));
dump($a);

// A hole in the middle
$a = [0, 1, 2, 3];
unset($a[1]);
var_dump(array_unshift($a, -1));
dump($a);

// The next index left by removed last elements is reset
$a = [0, 1, 2, 3, 4];
unset($a[4], $a[3]);
var_dump(array_unshift($a, -1));
$a[] = 'next';
dump($a);

// Without values, only the internal pointer and the next index are reset
$a = [0, 1, 2];
unset($a[2]);
end($a);
var_dump(array_unshift($a));
$a[] = 'next';
dump($a);

// An emptied packed array keeps its table
$a = [1];
array_pop($a);
var_dump(array_unshift($a, 1, 2));
$a[] = 'next';
dump($a);

// A foreach by reference keeps its position
$a = [1, 2, 3];
foreach ($a as &$v) {
    echo $v, "\n";
    if ($v == 2) {
        array_unshift($a, 0);
    }
}
unset($v);
dump($a);

// References stay shared and a copy is not modified
$x = 1;
$a = [&$x, 2];
$copy = $a;
array_unshift($a, 0);
$x = 10;
dump($a);
dump($copy);

// The array holds its own reference to the values
class Dtor {
    public function __destruct() {
        echo "destroyed\n";
    }
}
$o = new Dtor();
$a = [1];
array_unshift($a, $o);
unset($o);
echo "unshifted\n";
$a = null;
?>
--EXPECT--
int(16)
[-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14] key: 0
int(17)
[0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16] key: 0
int(4)
[-1,0,2,3] key: 0
int(4)
[-1,0,1,2,"next"] key: 0
int(2)
[0,1,"next"] key: 0
int(2)
[1,2,"next"] key: 0
1
2
3
[0,1,2,3] key: 0
[0,10,2] key: 0
[10,2] key: 0
unshifted
destroyed
