--TEST--
HTMLCollection::namedItem() and dimension handling for named accesses
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');

try {
    $dom->getElementsByTagName('root')[][1] = 1;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $dom->getElementsByTagName('root')[true];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    isset($dom->getElementsByTagName('root')[true]);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot append to Dom\HTMLCollection
Cannot access offset of type bool on Dom\HTMLCollection
Cannot access offset of type bool in isset or empty
