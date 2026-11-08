--TEST--
GH-25268 (Inserting a DOMNotation clone into a legacy document)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<!DOCTYPE root [<!NOTATION GIF SYSTEM "image/gif">]><root><x/></root>');
$notation = $doc->doctype->notations->getNamedItem('GIF');
$root = $doc->documentElement;

$tests = [
    'appendChild' => fn() => $root->appendChild(clone $notation),
    'insertBefore' => fn() => $root->insertBefore(clone $notation, $root->firstChild),
    'replaceChild' => fn() => $root->replaceChild(clone $notation, $root->firstChild),
    'append' => fn() => $root->append(clone $notation),
    'append original' => fn() => $root->append($notation),
];

foreach ($tests as $name => $test) {
    try {
        $test();
        echo $name, ': inserted', PHP_EOL;
    } catch (DOMException $e) {
        echo $name, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

echo $doc->saveXML($root), PHP_EOL;
?>
--EXPECT--
appendChild: DOMException: Hierarchy Request Error
insertBefore: DOMException: Hierarchy Request Error
replaceChild: DOMException: Hierarchy Request Error
append: DOMException: Hierarchy Request Error
append original: DOMException: Hierarchy Request Error
<root><x/></root>
