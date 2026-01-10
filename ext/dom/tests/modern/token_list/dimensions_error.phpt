--TEST--
TokenList: dimensions error
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$list = $dom->documentElement->classList;

$testOffsets = [
    new stdClass,
    [],
    fopen("php://output", "w"),
];

foreach ($testOffsets as $offset) {
    try {
        $list[$offset];
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        isset($list[$offset]);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    try {
        empty($list[$offset]);
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
}

try {
    $list[][0] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Cannot access offset of type stdClass on Dom\TokenList
Cannot access offset of type stdClass in isset or empty
Cannot access offset of type stdClass in isset or empty
Cannot access offset of type array on Dom\TokenList
Cannot access offset of type array in isset or empty
Cannot access offset of type array in isset or empty

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d

Warning: Resource ID#%d used as offset, casting to integer (%d) in %s on line %d
Cannot append to Dom\TokenList
