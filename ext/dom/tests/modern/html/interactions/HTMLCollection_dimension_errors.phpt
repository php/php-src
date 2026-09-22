--TEST--
HTMLCollection::namedItem() and dimension handling for named accesses
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString('<root/>');

try {
    $dom->getElementsByTagName('root')[][1] = 1;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $dom->getElementsByTagName('root')[true];
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    isset($dom->getElementsByTagName('root')[true]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot append to Dom\HTMLCollection
TypeError: Cannot access offset of type bool on Dom\HTMLCollection
TypeError: Cannot access offset of type bool in isset or empty
