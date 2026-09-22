--TEST--
GH-17201 (Dom\TokenList issues with interned string replace)
--EXTENSIONS--
dom
--INI--
opcache.protect_memory=1
--FILE--
<?php
$dom = DOM\XMLDocument::createFromString('<root class="AA B C"/>');
$element = $dom->documentElement;
$list = $element->classList;
$list->replace('AA', 'AB'); // Use interned string
foreach ($list as $entry) {
    var_dump($entry);
}
?>
--EXPECT--
string(2) "AB"
string(1) "B"
string(1) "C"
