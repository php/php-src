--TEST--
CSS Selectors - Pseudo classes: lang
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container lang="en">
    <p>1</p>
    <html xmlns="http://www.w3.org/1999/xhtml" lang="nl">
        <p>2</p>
    </html>
</container>
XML);

test_failure($dom, ':lang(en)', true);
test_failure($dom, ':lang(nl)', true);

?>
--EXPECT--
--- Selector: :lang(en) ---
Code 12 Invalid selector (Selectors. Not supported: lang)
--- Selector: :lang(nl) ---
Code 12 Invalid selector (Selectors. Not supported: lang)
