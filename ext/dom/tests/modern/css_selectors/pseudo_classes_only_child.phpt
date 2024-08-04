--TEST--
CSS Selectors - Pseudo classes: only-child
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <div class="only-child1">
        <p>Lonely</p>
    </div>
    <div class="only-child2">
        <p>With 2</p>
        <p>With 2</p>
    </div>
</container>
XML);

test_helper($dom, '.only-child1 p:only-child');
test_helper($dom, '.only-child2 p:only-child');

?>
--EXPECT--
--- Selector: .only-child1 p:only-child ---
<p>Lonely</p>
--- Selector: .only-child2 p:only-child ---
