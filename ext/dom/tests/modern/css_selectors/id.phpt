--TEST--
CSS Selectors - ID
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <test id="test1"/>
    <test/>
    <test id="test2"/>
    <test id="test3"/>
    <test xmlns:x="urn:x" x:id="test4"/>
</container>
XML);

test_helper($dom, '#test');
test_helper($dom, '#test1');
test_helper($dom, '#test2');
test_helper($dom, '#test3');
test_helper($dom, '#test4');

?>
--EXPECT--
--- Selector: #test ---
--- Selector: #test1 ---
<test id="test1"/>
--- Selector: #test2 ---
<test id="test2"/>
--- Selector: #test3 ---
<test id="test3"/>
--- Selector: #test4 ---
