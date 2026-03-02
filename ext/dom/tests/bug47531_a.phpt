--TEST--
Bug #47531 (No way of removing redundant xmlns: declarations)
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadXML(<<<XML
<container xmlns:foo="some:ns">
    <foo:first/>
    <second>
        <foo:child1/>
        <foo:child2/>
        <!-- comment -->
        <child3>
            <foo:child4/>
            <foo:child5>
                <p xmlns:foo="other:ns">
                    <span foo:foo="bar"/>
                    <span foo:foo="bar"/>
                </p>
                <foo:child6 foo:foo="bar">
                    <span foo:foo="bar"/>
                    <span foo:foo="bar"/>
                </foo:child6>
            </foo:child5>
        </child3>
        <child7 foo:foo="bar">
            <foo:child8/>
        </child7>
    </second>
</container>
XML);
$root = $doc->documentElement;
var_dump($root->removeAttribute("xmlns:foo"));
echo $doc->saveXML();

?>
--EXPECT--
bool(true)
<?xml version="1.0"?>
<container>
    <foo:first xmlns:foo="some:ns"/>
    <second>
        <foo:child1 xmlns:foo="some:ns"/>
        <foo:child2 xmlns:foo="some:ns"/>
        <!-- comment -->
        <child3>
            <foo:child4 xmlns:foo="some:ns"/>
            <foo:child5 xmlns:foo="some:ns">
                <p xmlns:foo="other:ns">
                    <span foo:foo="bar"/>
                    <span foo:foo="bar"/>
                </p>
                <foo:child6 foo:foo="bar">
                    <span foo:foo="bar"/>
                    <span foo:foo="bar"/>
                </foo:child6>
            </foo:child5>
        </child3>
        <child7 xmlns:foo="some:ns" foo:foo="bar">
            <foo:child8/>
        </child7>
    </second>
</container>
