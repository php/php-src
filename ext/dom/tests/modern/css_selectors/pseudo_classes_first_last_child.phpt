--TEST--
CSS Selectors - Pseudo classes: first/last child
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <?foo?>
    <!--bar-->
    <first />
    &amp;
    <last/>
    <![CDATA[skip me]]>
</container>
XML);

test_helper($dom, 'container > *:first-child');
test_helper($dom, 'container > *:last-child');

?>
--EXPECT--
--- Selector: container > *:first-child ---
<first/>
--- Selector: container > *:last-child ---
<last/>
