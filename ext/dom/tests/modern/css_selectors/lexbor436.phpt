--TEST--
Lexbor #436 (combinator after a compound-leading selector-taking pseudo-class checks only one element)
--EXTENSIONS--
dom
--FILE--
<?php

$siblings = Dom\HTMLDocument::createFromString(
    '<!DOCTYPE html><div><x></x><y></y><z></z><w></w></div>'
);
$nested = Dom\HTMLDocument::createFromString(
    '<!DOCTYPE html><ul><li>a<ol><li>b</li></ol></li></ul>'
);

$cases = [
    [$siblings, 'div > :is(y) ~ *'],
    [$siblings, 'div > :where(y) ~ *'],
    [$nested, ':is(ul) li'],
    [$nested, ':where(ul) li'],
    [$nested, ':not(ol) li'],
    [$nested, ':has(> li > ol) li'],
    [$nested, ':nth-child(1 of ul) li'],
    [$nested, 'body :is(li) li'],
];

foreach ($cases as [$dom, $selector]) {
    $result = [];
    foreach ($dom->querySelectorAll($selector) as $node) {
        $result[] = $node->localName . '(' . trim($node->firstChild?->textContent ?? '') . ')';
    }
    echo $selector, ': ', implode(' ', $result), "\n";
}

?>
--EXPECT--
div > :is(y) ~ *: z() w()
div > :where(y) ~ *: z() w()
:is(ul) li: li(a) li(b)
:where(ul) li: li(a) li(b)
:not(ol) li: li(a) li(b)
:has(> li > ol) li: li(a) li(b)
:nth-child(1 of ul) li: li(a) li(b)
body :is(li) li: li(b)
