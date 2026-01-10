--TEST--
CSS Selectors - Pseudo classes: only-of-type
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <div class="only-of-type1">
        <p>Alone</p>
    </div>
    <div class="only-of-type2">
        <p>With 2</p>
        <p>With 2</p>
    </div>
    <div class="only-of-type3">
        <p>With 2</p>
        <div/>
        <p>With 2</p>
    </div>
</container>
XML);

test_helper($dom, '.only-of-type1 p:only-of-type');
test_helper($dom, '.only-of-type2 p:only-of-type');
test_helper($dom, '.only-of-type3 p:only-of-type');

?>
--EXPECT--
--- Selector: .only-of-type1 p:only-of-type ---
<p>Alone</p>
--- Selector: .only-of-type2 p:only-of-type ---
--- Selector: .only-of-type3 p:only-of-type ---
