--TEST--
Random: Randomizer: The Mt19937 engine and pickArrayKeys are consistent with array_rand()
--FILE--
<?php

use Random\Engine\Mt19937;
use Random\Randomizer;

$map = ['foo' => 0, 'bar' => 1, 'baz' => 2];
$list = range(1, 10);

mt_srand(1234);

$mapPickOneFunc = array_rand($map, 1);
$mapPickTwoFunc = array_rand($map, 2);

$listPickOneFunc = array_rand($list, 1);
$listPickTwoFunc = array_rand($list, 2);

$randomizer = new Randomizer(new Mt19937(1234));

[$mapPickOneMethod] = $randomizer->pickArrayKeys($map, 1);
$mapPickTwoMethod = $randomizer->pickArrayKeys($map, 2);

[$listPickOneMethod] = $randomizer->pickArrayKeys($list, 1);
$listPickTwoMethod = $randomizer->pickArrayKeys($list, 2);

if ($mapPickOneFunc !== $mapPickOneMethod) {
    var_dump($mapPickOneFunc, $mapPickOneMethod);

    die('failure: mapPickOne');
}

if ($mapPickTwoFunc !== $mapPickTwoMethod) {
    var_dump($mapPickTwoFunc, $mapPickTwoMethod);

    die('failure: mapPickTwo');
}

if ($listPickOneFunc !== $listPickOneMethod) {
    var_dump($listPickOneFunc, $listPickOneMethod);

    die('failure: listPickOne');
}

if ($listPickTwoFunc !== $listPickTwoMethod) {
    var_dump($listPickTwoFunc, $listPickOneMethod);

    die('failure: listPickTwo');
}

die('success');

?>
--EXPECT--
success
