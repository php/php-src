--TEST--
CSS Selectors - Pseudo elements
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <a/>
</container>
XML);

// Pseudo-elements can't work in a static DOM
test_failure($dom, 'a::after');
test_failure($dom, 'a::before');
test_failure($dom, 'a::backdrop');
test_failure($dom, 'a::first-letter');
test_failure($dom, 'a::first-line');
test_failure($dom, 'a::grammar-error');
test_failure($dom, 'a::inactive-selection');
test_failure($dom, 'a::marker');
test_failure($dom, 'a::placeholder');
test_failure($dom, 'a::selection');
test_failure($dom, 'a::spelling-error');
test_failure($dom, 'a::target-text');

?>
--EXPECT--
--- Selector: a::after ---
Code 12 Invalid selector (Selectors. Not supported: after)
--- Selector: a::before ---
Code 12 Invalid selector (Selectors. Not supported: before)
--- Selector: a::backdrop ---
Code 12 Invalid selector (Selectors. Not supported: backdrop)
--- Selector: a::first-letter ---
Code 12 Invalid selector (Selectors. Not supported: first-letter)
--- Selector: a::first-line ---
Code 12 Invalid selector (Selectors. Not supported: first-line)
--- Selector: a::grammar-error ---
Code 12 Invalid selector (Selectors. Not supported: grammar-error)
--- Selector: a::inactive-selection ---
Code 12 Invalid selector (Selectors. Not supported: inactive-selection)
--- Selector: a::marker ---
Code 12 Invalid selector (Selectors. Not supported: marker)
--- Selector: a::placeholder ---
Code 12 Invalid selector (Selectors. Not supported: placeholder)
--- Selector: a::selection ---
Code 12 Invalid selector (Selectors. Not supported: selection)
--- Selector: a::spelling-error ---
Code 12 Invalid selector (Selectors. Not supported: spelling-error)
--- Selector: a::target-text ---
Code 12 Invalid selector (Selectors. Not supported: target-text)
