--TEST--
CSS Selectors - Pseudo classes: checked
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="http://www.w3.org/1999/xhtml">
    <input type="checkbox" checked="checked" />
    <input type="radio" checked="checked" />
    <option selected="" />
    <option xmlns="" selected="" />
    <input/>
</html>
XML);

test_helper($dom, ':checked');

?>
--EXPECT--
--- Selector: :checked ---
<input xmlns="http://www.w3.org/1999/xhtml" type="checkbox" checked="checked" />
<input xmlns="http://www.w3.org/1999/xhtml" type="radio" checked="checked" />
<option xmlns="http://www.w3.org/1999/xhtml" selected=""></option>
