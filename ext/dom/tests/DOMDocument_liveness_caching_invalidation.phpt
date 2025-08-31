--TEST--
DOMDocument liveness caching invalidation by textContent
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML('<root><e id="1"/><e id="2"/><e id="3"/><e id="4"/><e id="5"/></root>');
$root = $doc->documentElement;

$i = 0;

echo "-- Overwrite during iteration --\n";

foreach ($doc->getElementsByTagName('e') as $node) {
    if ($i++ == 2) {
        $root->textContent = 'overwrite';
    }
    var_dump($node->tagName, $node->getAttribute('id'));
}

echo "-- Empty iteration --\n";
foreach ($doc->getElementsByTagName('e') as $node) {
    echo "Should not execute\n";
}

echo "-- After adding an element again --\n";
$root->appendChild(new DOMElement('e'));
foreach ($doc->getElementsByTagName('e') as $node) {
    echo "Should execute once\n";
}
?>
--EXPECT--
-- Overwrite during iteration --
string(1) "e"
string(1) "1"
string(1) "e"
string(1) "2"
string(1) "e"
string(1) "3"
-- Empty iteration --
-- After adding an element again --
Should execute once
