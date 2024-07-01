--TEST--
CSS Selectors - Pseudo classes: optional/required
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="http://www.w3.org/1999/xhtml">
    <input type="checkbox" required="required" />
    <select required="required" />
    <textarea required="" />
    <input type="checkbox" />
    <select />
    <textarea />
    <input xmlns=""/>
    <input xmlns="" required="" />
</html>
XML);

test_helper($dom, ':required');
test_helper($dom, ':optional');

?>
--EXPECT--
--- Selector: :required ---
<input xmlns="http://www.w3.org/1999/xhtml" type="checkbox" required="required" />
<select xmlns="http://www.w3.org/1999/xhtml" required="required"></select>
<textarea xmlns="http://www.w3.org/1999/xhtml" required=""></textarea>
--- Selector: :optional ---
<input xmlns="http://www.w3.org/1999/xhtml" type="checkbox" />
<select xmlns="http://www.w3.org/1999/xhtml"></select>
<textarea xmlns="http://www.w3.org/1999/xhtml"></textarea>
