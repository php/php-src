--TEST--
Test array argument in-place optimization
--EXTENSIONS--
zend_test
--FILE--
<?php

enum EN
{
    case Y;
    case N;
}

$listInt = [];
$listStr = [];
$mapInt = [];
$mapObj = [];
if (time() > 10) { // always true, but not const expr as time() is not CTE function
    $listInt[] = 1;
    $listInt[] = 2;
    $listStr[] = 'a';
    $mapInt[1] = 0;
    $mapObj['e'] = EN::Y;
    $mapObj['f'] = EN::N;
}

$anotherList = [8, 9];

// the arrays now have GC_IMMUTABLE and GC_PERSISTENT flags cleared

echo "*** array_merge ***\n";

// const empty 2nd array
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_merge($listInt, []);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($listStr);
$listStr = array_merge($listStr, []);
$ptrAfter = zend_get_array_ptr($listStr);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapInt);
$mapInt = array_merge($mapInt, []);
$ptrAfter = zend_get_array_ptr($mapInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_merge($mapObj, []);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

// const non-empty 2nd array
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_merge($listInt, [4]);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($listStr);
$listStr = array_merge($listStr, [4]);
$ptrAfter = zend_get_array_ptr($listStr);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapInt);
$mapInt = array_merge($mapInt, [4]);
$ptrAfter = zend_get_array_ptr($mapInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_merge($mapObj, [4]);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

// non-const non-empty 2nd array
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_merge($listInt, $anotherList);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($listStr);
$listStr = array_merge($listStr, $anotherList);
$ptrAfter = zend_get_array_ptr($listStr);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapInt);
$mapInt = array_merge($mapInt, $anotherList);
$ptrAfter = zend_get_array_ptr($mapInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_merge($mapObj, $anotherList);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

// non-1st argument as a result
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_merge($anotherList, $listInt);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($listStr);
$listStr = array_merge($anotherList, $listStr);
$ptrAfter = zend_get_array_ptr($listStr);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapInt);
$mapInt = array_merge($anotherList, $mapInt);
$ptrAfter = zend_get_array_ptr($mapInt);
var_dump($ptrAfter === $ptrBefore);

$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_merge($anotherList, $mapObj);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

echo "---\n";
foreach ($listInt as $v) {
    $ptrBefore = zend_get_array_ptr($listInt);
    $listInt = array_merge($listInt, [$v]); // 2nd and 3rd iteration must not copy the array
    $ptrAfter = zend_get_array_ptr($listInt);
    var_dump($ptrAfter === $ptrBefore);

    if ($v === 1) { // 3rd iteration
        break;
    }
}
foreach (array_keys($listInt) as $k) {
    $ptrBefore = zend_get_array_ptr($listInt);
    $listInt = array_merge($listInt, [$listInt[$k]]); // array must never be copied
    $ptrAfter = zend_get_array_ptr($listInt);
    var_dump($ptrAfter === $ptrBefore);

    if ($listInt[$k] === 1) { // 3rd iteration
        break;
    }
}

// TODO array_merge should be optimized to always return the 1st array zval if there is no element to be added

print_r($anotherList); // must not be modified
print_r($listInt);
print_r($listStr);
print_r($mapInt);
print_r($mapObj);

$listInt = array_slice($listInt, 0, 2, true);
$listStr = array_slice($listStr, 0, 1, true);
$mapInt = array_slice($mapInt, 0, 1, true);
$mapObj = array_slice($mapObj, 0, 2, true);


echo "*** array merge with unpacking ***\n";
// TODO

echo "*** array_diff family ***\n";
// TODO array_diff family should be optimized - https://github.com/php/php-src/pull/11060#discussion_r1175022196

echo "*** array_intersect family ***\n";
$oneListInt = array_slice($listInt, 0, 1, true);
$oneMapObj = array_slice($mapObj, 0, 1, true);

$listInt[] = -1;
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_intersect($listInt, $oneListInt);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$mapObj[] = -1;
$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_intersect($mapObj, $oneMapObj);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

$listInt[] = -1;
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_intersect_assoc($listInt, $oneListInt);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$mapObj[] = -1;
$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_intersect_assoc($mapObj, $oneMapObj);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

$listInt[] = -1;
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_intersect_key($listInt, $oneListInt);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$listInt[] = -1;
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_intersect_ukey($listInt, $oneListInt, fn ($k1, $k2) => $k1 <=> $k2);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$mapObj[] = -1;
$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_intersect_ukey($mapObj, $oneListInt, fn ($k1, $k2) => $k1 <=> $k2);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

echo "*** array_unique ***\n";

$listInt[] = end($listInt);
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_unique($listInt);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$mapObj[] = end($mapObj);
$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_unique($mapObj, SORT_REGULAR);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

echo "*** array_replace ***\n";

$listInt[] = end($listInt);
$ptrBefore = zend_get_array_ptr($listInt);
$listInt = array_replace($listInt, ['*']);
$ptrAfter = zend_get_array_ptr($listInt);
var_dump($ptrAfter === $ptrBefore);

$mapObj[] = end($mapObj);
$ptrBefore = zend_get_array_ptr($mapObj);
$mapObj = array_replace($mapObj, ['*']);
$ptrAfter = zend_get_array_ptr($mapObj);
var_dump($ptrAfter === $ptrBefore);

print_r($listInt);
print_r($mapObj);

?>
--EXPECT--
*** array_merge ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
---
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
Array
(
    [0] => 8
    [1] => 9
)
Array
(
    [0] => 8
    [1] => 9
    [2] => 1
    [3] => 2
    [4] => 4
    [5] => 8
    [6] => 9
    [7] => 8
    [8] => 9
    [9] => 1
    [10] => 8
    [11] => 9
    [12] => 1
)
Array
(
    [0] => 8
    [1] => 9
    [2] => a
    [3] => 4
    [4] => 8
    [5] => 9
)
Array
(
    [0] => 8
    [1] => 9
    [2] => 0
    [3] => 4
    [4] => 8
    [5] => 9
)
Array
(
    [0] => 8
    [1] => 9
    [e] => EN Enum
        (
            [name] => Y
        )

    [f] => EN Enum
        (
            [name] => N
        )

    [2] => 4
    [3] => 8
    [4] => 9
)
*** array merge with unpacking ***
*** array_diff family ***
*** array_intersect family ***
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
*** array_unique ***
bool(true)
bool(true)
*** array_replace ***
bool(true)
bool(true)
Array
(
    [0] => *
    [1] => 8
)
Array
(
    [0] => *
    [3] => 8
)
