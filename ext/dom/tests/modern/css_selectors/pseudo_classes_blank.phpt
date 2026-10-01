--TEST--
CSS Selectors - Pseudo classes: blank
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container/>
XML);

try {
    test_helper($dom, ':blank');
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Selector: :blank ---
DOMException: :blank selector is not implemented because CSSWG has not yet decided its semantics (https://github.com/w3c/csswg-drafts/issues/1967)
