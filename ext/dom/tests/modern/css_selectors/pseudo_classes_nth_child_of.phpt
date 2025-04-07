--TEST--
CSS Selectors - Pseudo classes: nth-child of
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <h2 class="hi">1</h2>
    <h2>2</h2>
    <h2 class="hi">3</h2>
    <h2 class="hi">4</h2>
    <h2>5</h2>
    <h2 class="hi">6</h2>
</container>
XML);

test_helper($dom, 'h2:nth-child(even of .hi)');
test_helper($dom, 'h2.hi:nth-child(even)');
test_helper($dom, 'h2:nth-child(odd of .hi)');
test_helper($dom, 'h2.hi:nth-child(odd)');

test_helper($dom, 'h2:nth-last-child(even of .hi)');
test_helper($dom, 'h2.hi:nth-last-child(even)');
test_helper($dom, 'h2:nth-last-child(odd of .hi)');
test_helper($dom, 'h2.hi:nth-last-child(odd)');

?>
--EXPECT--
--- Selector: h2:nth-child(even of .hi) ---
<h2 class="hi">3</h2>
<h2 class="hi">6</h2>
--- Selector: h2.hi:nth-child(even) ---
<h2 class="hi">4</h2>
<h2 class="hi">6</h2>
--- Selector: h2:nth-child(odd of .hi) ---
<h2 class="hi">1</h2>
<h2 class="hi">4</h2>
--- Selector: h2.hi:nth-child(odd) ---
<h2 class="hi">1</h2>
<h2 class="hi">3</h2>
--- Selector: h2:nth-last-child(even of .hi) ---
<h2 class="hi">1</h2>
<h2 class="hi">4</h2>
--- Selector: h2.hi:nth-last-child(even) ---
<h2 class="hi">1</h2>
<h2 class="hi">3</h2>
--- Selector: h2:nth-last-child(odd of .hi) ---
<h2 class="hi">3</h2>
<h2 class="hi">6</h2>
--- Selector: h2.hi:nth-last-child(odd) ---
<h2 class="hi">4</h2>
<h2 class="hi">6</h2>
