--TEST--
Dom\Element::insertAdjacentHTML() with XML nodes
--EXTENSIONS--
dom
--FILE--
<?php

const POSITIONS = [
    Dom\AdjacentPosition::BeforeBegin,
    Dom\AdjacentPosition::AfterBegin,
    Dom\AdjacentPosition::BeforeEnd,
    Dom\AdjacentPosition::AfterEnd,
];

function test(string $xml) {
    echo "=== XML ($xml) ===\n";

    foreach (POSITIONS as $position) {
        echo "--- Position ", $position->name, " ---\n";

        $dom = Dom\XMLDocument::createFromString('<root xmlns:x="urn:x"><div/></root>');
        $div = $dom->documentElement->firstChild;
        $div->append("Sample text");

        $div->insertAdjacentHTML($position, $xml);

        echo $dom->saveXML(), "\n";
        var_dump($div->childNodes->length);
        var_dump($dom->documentElement->childNodes->length);
    }
}

test('<x:x><y xmlns:x="urn:x2"/></x:x><nons/>');
test('<?pi node?><!-- comment -->&amp;');
test('text node');

?>
--EXPECT--
=== XML (<x:x><y xmlns:x="urn:x2"/></x:x><nons/>) ===
--- Position BeforeBegin ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><x:x><y xmlns:x="urn:x2"/></x:x><nons/><div>Sample text</div></root>
int(1)
int(3)
--- Position AfterBegin ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div><x:x><y xmlns:x="urn:x2"/></x:x><nons/>Sample text</div></root>
int(3)
int(1)
--- Position BeforeEnd ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>Sample text<x:x><y xmlns:x="urn:x2"/></x:x><nons/></div></root>
int(3)
int(1)
--- Position AfterEnd ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>Sample text</div><x:x><y xmlns:x="urn:x2"/></x:x><nons/></root>
int(1)
int(3)
=== XML (<?pi node?><!-- comment -->&amp;) ===
--- Position BeforeBegin ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><?pi node?><!-- comment -->&amp;<div>Sample text</div></root>
int(1)
int(4)
--- Position AfterBegin ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div><?pi node?><!-- comment -->&amp;Sample text</div></root>
int(4)
int(1)
--- Position BeforeEnd ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>Sample text<?pi node?><!-- comment -->&amp;</div></root>
int(4)
int(1)
--- Position AfterEnd ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>Sample text</div><?pi node?><!-- comment -->&amp;</root>
int(1)
int(4)
=== XML (text node) ===
--- Position BeforeBegin ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x">text node<div>Sample text</div></root>
int(1)
int(2)
--- Position AfterBegin ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>text nodeSample text</div></root>
int(2)
int(1)
--- Position BeforeEnd ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>Sample texttext node</div></root>
int(2)
int(1)
--- Position AfterEnd ---
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns:x="urn:x"><div>Sample text</div>text node</root>
int(1)
int(2)
