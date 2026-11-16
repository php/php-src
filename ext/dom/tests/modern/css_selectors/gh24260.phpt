--TEST--
GH-24260 (querySelectorAll() returns duplicate elements for selector lists)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(
    '<!DOCTYPE html><div><p class="a b">one</p><p class="b">two</p></div>'
);

foreach ([$dom, $dom->querySelector('div')] as $context) {
    foreach (['.a, .b', '.a, .b, .a.b', 'p, :is(.a, .b)'] as $selector) {
        $texts = [];
        foreach ($context->querySelectorAll($selector) as $node) {
            $texts[] = $node->textContent;
        }
        echo $selector, ': ', implode(' ', $texts), "\n";
    }
}

?>
--EXPECT--
.a, .b: one two
.a, .b, .a.b: one two
p, :is(.a, .b): one two
.a, .b: one two
.a, .b, .a.b: one two
p, :is(.a, .b): one two
