--TEST--
GH-25210 (querySelectorAll() returns duplicates for selector lists with combinators)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(
    '<!DOCTYPE html><ul><li>a<ol><li id="x">b</li></ol></li></ul>'
);

foreach ([$dom, $dom->querySelector('ul')] as $context) {
    foreach (['ul li, ol li', 'ol li, ul li', 'li, li', 'ul li, ol > li', '#x, ul li'] as $selector) {
        $ids = [];
        foreach ($context->querySelectorAll($selector) as $node) {
            $ids[] = $node->id ?: $node->textContent;
        }
        echo $selector, ': ', implode(' ', $ids), "\n";
    }
}

?>
--EXPECT--
ul li, ol li: ab x
ol li, ul li: ab x
li, li: ab x
ul li, ol > li: ab x
#x, ul li: ab x
ul li, ol li: ab x
ol li, ul li: ab x
li, li: ab x
ul li, ol > li: ab x
#x, ul li: ab x
