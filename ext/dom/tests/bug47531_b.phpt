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
    <foo:second xmlns:foo="some:ns2">
        <foo:child1/>
        <foo:child2/>
        <!-- comment -->
        <child3>
            <foo:child4/>
            <foo:child5 xmlns:foo="some:ns3">
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
    </foo:second>
</container>
XML);
$root = $doc->documentElement;
$elem = $root->firstElementChild->nextElementSibling;
var_dump($elem->removeAttribute("xmlns:foo"));
echo $doc->saveXML();

?>
--EXPECT--
bool(true)
<?xml version="1.0"?>
<container xmlns:foo="some:ns">
    <foo:first/>
    <foo:second xmlns:foo="some:ns2">
        <foo:child1/>
        <foo:child2/>
        <!-- comment -->
        <child3>
            <foo:child4/>
            <foo:child5 xmlns:foo="some:ns3">
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
    </foo:second>
</container>
