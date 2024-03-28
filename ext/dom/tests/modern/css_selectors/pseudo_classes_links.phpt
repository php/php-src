--TEST--
CSS Selectors - Pseudo classes: links
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <a href="http://example.com">Link</a>
    <a xmlns="http://www.w3.org/1999/xhtml" href="http://example.com">Link</a>
    <area xmlns="http://www.w3.org/1999/xhtml" href="http://example.com">Link</area>
</container>
XML);

test_helper($dom, ':any-link');
test_helper($dom, ':link');
test_helper($dom, 'a:not(:any-link)');

?>
--EXPECT--
--- Selector: :any-link ---
<a xmlns="http://www.w3.org/1999/xhtml" href="http://example.com">Link</a>
<area xmlns="http://www.w3.org/1999/xhtml" href="http://example.com">Link</area>
--- Selector: :link ---
<a xmlns="http://www.w3.org/1999/xhtml" href="http://example.com">Link</a>
<area xmlns="http://www.w3.org/1999/xhtml" href="http://example.com">Link</area>
--- Selector: a:not(:any-link) ---
<a href="http://example.com">Link</a>
