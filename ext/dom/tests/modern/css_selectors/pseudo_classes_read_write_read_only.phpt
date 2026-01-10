--TEST--
CSS Selectors - Pseudo classes: read-write/read-only
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="http://www.w3.org/1999/xhtml">
    <input type="text" readonly="" />
    <textarea readonly="" />
    <input type="text" disabled="" />
    <textarea disabled="" />
    <input type="text" xmlns="" />
    <textarea xmlns="" />
    <input type="text" />
    <textarea />
    <p contenteditable="" />
    <p contenteditable="false" />
</html>
XML);

test_helper($dom, ':read-write');
test_helper($dom, ':read-only');

?>
--EXPECT--
--- Selector: :read-write ---
<input xmlns="http://www.w3.org/1999/xhtml" type="text" />
<textarea xmlns="http://www.w3.org/1999/xhtml"></textarea>
<p xmlns="http://www.w3.org/1999/xhtml" contenteditable=""></p>
--- Selector: :read-only ---
<html xmlns="http://www.w3.org/1999/xhtml">
    <input type="text" readonly="" />
    <textarea readonly=""></textarea>
    <input type="text" disabled="" />
    <textarea disabled=""></textarea>
    <input xmlns="" type="text"/>
    <textarea xmlns=""/>
    <input type="text" />
    <textarea></textarea>
    <p contenteditable=""></p>
    <p contenteditable="false"></p>
</html>
<input xmlns="http://www.w3.org/1999/xhtml" type="text" readonly="" />
<textarea xmlns="http://www.w3.org/1999/xhtml" readonly=""></textarea>
<input xmlns="http://www.w3.org/1999/xhtml" type="text" disabled="" />
<textarea xmlns="http://www.w3.org/1999/xhtml" disabled=""></textarea>
<input xmlns="" type="text"/>
<textarea xmlns=""/>
<p xmlns="http://www.w3.org/1999/xhtml" contenteditable="false"></p>
