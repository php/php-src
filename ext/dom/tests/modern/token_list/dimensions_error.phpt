--TEST--
TokenList: dimensions error
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$list = $dom->documentElement->classList;

try {
    $list[new stdClass];
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    isset($list[new stdClass]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    empty($list[new stdClass]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $list[][0] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot access offset of type stdClass on DOM\TokenList
Cannot access offset of type stdClass in isset or empty
Cannot access offset of type stdClass in isset or empty
Cannot append to DOM\TokenList
