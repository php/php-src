--TEST--
Dom\Element::insertAdjacentHTML() with HTML nodes - error conditions
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$element = $dom->createElement('root');

echo "--- BeforeBegin no parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::BeforeBegin, "test");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- AfterEnd no parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::AfterEnd, "test");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$dom->appendChild($element);

echo "--- BeforeBegin document parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::BeforeBegin, "test");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- AfterEnd document parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::AfterEnd, "test");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
--- BeforeBegin no parent ---
No Modification Allowed Error
--- AfterEnd no parent ---
No Modification Allowed Error
--- BeforeBegin document parent ---
No Modification Allowed Error
--- AfterEnd document parent ---
No Modification Allowed Error
