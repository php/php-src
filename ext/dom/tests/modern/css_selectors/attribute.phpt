--TEST--
CSS Selectors - Attribute
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <a title="http://example.com" lang="en-us"/>
    <a title="http://example.be"/>
    <a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
    <a title="ftp://example.nl" lang="nl-be"/>
</container>
XML);

echo "=== Case sensitive ===\n";

test_helper($dom, 'a[title]');
test_helper($dom, 'a[title="http://example.com"]');
test_helper($dom, 'a[title="http://example."]');
test_helper($dom, 'a[title*="example"]');
test_helper($dom, 'a[title*=""]');
test_helper($dom, 'a[title^="HTTP"]');
test_helper($dom, 'a[title^="http"]');
test_helper($dom, 'a[title^="http"][title$=".be"]');
test_helper($dom, 'a[title$=".com"]');
test_helper($dom, 'a[title$=".foo"]');
test_helper($dom, 'a[lang|="nl"]');
test_helper($dom, 'a[lang|="nl-be"]');
test_helper($dom, 'a[tokens~="def"]');
test_helper($dom, 'a[tokens~="de"]');
test_helper($dom, 'a[tokens~="def ghi"]');

echo "=== Case insensitive ===\n";

test_helper($dom, 'a[title]');
test_helper($dom, 'a[title="http://example.COM" i]');
test_helper($dom, 'a[title="http://EXAMPLE." i]');
test_helper($dom, 'a[title*="ExAmPlE" i]');
test_helper($dom, 'a[title^="HTTP" i]');
test_helper($dom, 'a[title^="HTTP" i][title$=".be"]');
test_helper($dom, 'a[title$=".COM" i]');
test_helper($dom, 'a[lang|="NL" i]');
test_helper($dom, 'a[lang|="NL-BE" i]');
test_helper($dom, 'a[tokens~="DE" i]');
test_helper($dom, 'a[tokens~="DEF" i]');
test_helper($dom, 'a[tokens~="DEF GHI" i]');

?>
--EXPECT--
=== Case sensitive ===
--- Selector: a[title] ---
<a title="http://example.com" lang="en-us"/>
<a title="http://example.be"/>
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[title="http://example.com"] ---
<a title="http://example.com" lang="en-us"/>
--- Selector: a[title="http://example."] ---
--- Selector: a[title*="example"] ---
<a title="http://example.com" lang="en-us"/>
<a title="http://example.be"/>
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[title*=""] ---
--- Selector: a[title^="HTTP"] ---
--- Selector: a[title^="http"] ---
<a title="http://example.com" lang="en-us"/>
<a title="http://example.be"/>
--- Selector: a[title^="http"][title$=".be"] ---
<a title="http://example.be"/>
--- Selector: a[title$=".com"] ---
<a title="http://example.com" lang="en-us"/>
--- Selector: a[title$=".foo"] ---
--- Selector: a[lang|="nl"] ---
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[lang|="nl-be"] ---
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[tokens~="def"] ---
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
--- Selector: a[tokens~="de"] ---
--- Selector: a[tokens~="def ghi"] ---
=== Case insensitive ===
--- Selector: a[title] ---
<a title="http://example.com" lang="en-us"/>
<a title="http://example.be"/>
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[title="http://example.COM" i] ---
<a title="http://example.com" lang="en-us"/>
--- Selector: a[title="http://EXAMPLE." i] ---
--- Selector: a[title*="ExAmPlE" i] ---
<a title="http://example.com" lang="en-us"/>
<a title="http://example.be"/>
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[title^="HTTP" i] ---
<a title="http://example.com" lang="en-us"/>
<a title="http://example.be"/>
--- Selector: a[title^="HTTP" i][title$=".be"] ---
<a title="http://example.be"/>
--- Selector: a[title$=".COM" i] ---
<a title="http://example.com" lang="en-us"/>
--- Selector: a[lang|="NL" i] ---
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[lang|="NL-BE" i] ---
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
<a title="ftp://example.nl" lang="nl-be"/>
--- Selector: a[tokens~="DE" i] ---
--- Selector: a[tokens~="DEF" i] ---
<a title="ftp://example.be" lang="nl-be" tokens="abc def ghi"/>
--- Selector: a[tokens~="DEF GHI" i] ---
