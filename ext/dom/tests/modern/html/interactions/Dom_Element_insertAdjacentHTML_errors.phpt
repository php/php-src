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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- AfterEnd no parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::AfterEnd, "test");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$dom->appendChild($element);

echo "--- BeforeBegin document parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::BeforeBegin, "test");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "--- AfterEnd document parent ---\n";

try {
    $element->insertAdjacentHTML(Dom\AdjacentPosition::AfterEnd, "test");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
--- BeforeBegin no parent ---
DOMException: No Modification Allowed Error
--- AfterEnd no parent ---
DOMException: No Modification Allowed Error
--- BeforeBegin document parent ---
DOMException: No Modification Allowed Error
--- AfterEnd document parent ---
DOMException: No Modification Allowed Error
