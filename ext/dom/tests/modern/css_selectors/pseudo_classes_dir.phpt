--TEST--
CSS Selectors - Pseudo classes: dir
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container dir="rtl">
    <p>1</p>
    <html xmlns="http://www.w3.org/1999/xhtml" dir="ltr">
        <p>2</p>
    </html>
</container>
XML);

test_failure($dom, ':dir(rtl)', true);
test_failure($dom, ':dir(ltr)', true);

?>
--EXPECT--
--- Selector: :dir(rtl) ---
Code 12 Invalid selector (Selectors. Not supported: dir)
--- Selector: :dir(ltr) ---
Code 12 Invalid selector (Selectors. Not supported: dir)
