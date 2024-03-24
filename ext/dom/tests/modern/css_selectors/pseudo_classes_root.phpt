--TEST--
CSS Selectors - Pseudo classes: root
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container/>
XML);

test_helper($dom, ':root', true);
$fragment = $dom->createDocumentFragment();
$fragment->appendXML('<div><p></p></div>');
test_helper($fragment, ':root', true);
test_helper($dom->createElement("foo"), ':root', true);

test_helper($dom->createElement("not-a-root"), ":root", true);

?>
--EXPECT--
--- Selector: :root ---
container
--- Selector: :root ---
div
--- Selector: :root ---
--- Selector: :root ---
