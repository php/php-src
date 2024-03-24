--TEST--
CSS Selectors - Pseudo classes: has
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <div>
        <p class="foo"/>
    </div>
    <div>
        <p/>
    </div>
</container>
XML);

test_helper($dom, 'div:has(p.foo)');
test_helper($dom, 'div:has(:not(p.foo))');

?>
--EXPECT--
--- Selector: div:has(p.foo) ---
<div>
        <p class="foo"/>
    </div>
--- Selector: div:has(:not(p.foo)) ---
<div>
        <p/>
    </div>
