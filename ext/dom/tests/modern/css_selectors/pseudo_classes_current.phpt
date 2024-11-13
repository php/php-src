--TEST--
CSS Selectors - Pseudo classes: current
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="http://www.w3.org/1999/xhtml">
    <div/>
</html>
XML);

test_helper($dom, ':current(div)');

?>
--EXPECT--
--- Selector: :current(div) ---
<div xmlns="http://www.w3.org/1999/xhtml"></div>
