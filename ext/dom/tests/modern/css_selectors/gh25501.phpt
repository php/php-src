--TEST--
GH-25501 (querySelector() rejects a selector whose identifier starts with a non-ASCII character)
--CREDITS--
phcorp
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(
    '<p id="été">a</p><p class="élan">b</p><p class="café">c</p><p class="cafés">d</p>'
    . '<p class="Über">e</p><p class="ͽx">f</p><p class="a×">g</p>',
    LIBXML_NOERROR
);

$selectors = [
    '#\e9 t\e9',
    '#été',
    'p#été',
    '[id=été]',
    '.élan',
    '.café',
    '.cafés',
    '.Über',
    '.ͽx',
    '.a×',
    '#×',
    '.÷x',
];

foreach ($selectors as $selector) {
    echo $selector, ': ';
    try {
        var_dump($dom->querySelector($selector)?->textContent);
    } catch (DOMException $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
#\e9 t\e9: string(1) "a"
#été: string(1) "a"
p#été: string(1) "a"
[id=été]: string(1) "a"
.élan: string(1) "b"
.café: string(1) "c"
.cafés: string(1) "d"
.Über: string(1) "e"
.ͽx: string(1) "f"
.a×: DOMException: Invalid selector (Selectors. Unexpected token: ×)
#×: DOMException: Invalid selector (Selectors. Unexpected token: #)
.÷x: DOMException: Invalid selector (Selectors. Unexpected token: ÷)
