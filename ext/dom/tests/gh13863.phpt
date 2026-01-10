--TEST--
GH-13863 (Removal during NodeList iteration breaks loop)
--EXTENSIONS--
dom
--FILE--
<?php

enum DomType {
    case Legacy;
    case Modern;
}

function createTestDoc(DomType $type) {
    $xml = <<<EOXML
    <x>
        <item1 />
        <item2 />
        <item3 />
        <item4 />
        <item5 />
    </x>
    EOXML;

    if ($type === DomType::Legacy) {
        $doc = new DOMDocument();
        $doc->preserveWhiteSpace = false;
        $doc->loadXML($xml);
    } else {
        $doc = Dom\XMLDocument::createFromString($xml, LIBXML_NOBLANKS);
    }

    return $doc;
}

foreach ([DomType::Legacy, DomType::Modern] as $case) {
    $name = match ($case) {
        DomType::Legacy => 'Legacy',
        DomType::Modern => 'Modern',
    };

    echo "--- $name test remove index 2 at index 2 ---\n";

    $doc = createTestDoc($case);

    foreach ($doc->documentElement->childNodes as $i => $node) {
        var_dump($doc->documentElement->childNodes->item($i) === $node);
        var_dump($i, $node->localName);
        if ($i === 2) {
            $node->remove();
        }
    }

    echo $doc->saveXML(), "\n";

    echo "--- $name test remove index 1 at index 2 ---\n";

    $doc = createTestDoc($case);

    foreach ($doc->documentElement->childNodes as $i => $node) {
        var_dump($doc->documentElement->childNodes->item($i) === $node);
        var_dump($i, $node->localName);
        if ($i === 2) {
            $doc->documentElement->childNodes[1]->remove();
        }
    }

    echo $doc->saveXML(), "\n";

    echo "--- $name test remove all ---\n";

    $doc = createTestDoc($case);

    foreach ($doc->documentElement->childNodes as $i => $node) {
        var_dump($doc->documentElement->childNodes->item($i) === $node);
        var_dump($i, $node->localName);
        $node->remove();
    }

    echo $doc->saveXML(), "\n";
}

?>
--EXPECT--
--- Legacy test remove index 2 at index 2 ---
bool(true)
int(0)
string(5) "item1"
bool(true)
int(1)
string(5) "item2"
bool(true)
int(2)
string(5) "item3"
<?xml version="1.0"?>
<x><item1/><item2/><item4/><item5/></x>

--- Legacy test remove index 1 at index 2 ---
bool(true)
int(0)
string(5) "item1"
bool(true)
int(1)
string(5) "item2"
bool(true)
int(2)
string(5) "item3"
bool(false)
int(3)
string(5) "item4"
bool(false)
int(4)
string(5) "item5"
<?xml version="1.0"?>
<x><item1/><item3/><item4/><item5/></x>

--- Legacy test remove all ---
bool(true)
int(0)
string(5) "item1"
<?xml version="1.0"?>
<x><item2/><item3/><item4/><item5/></x>

--- Modern test remove index 2 at index 2 ---
bool(true)
int(0)
string(5) "item1"
bool(true)
int(1)
string(5) "item2"
bool(true)
int(2)
string(5) "item3"
bool(true)
int(3)
string(5) "item5"
<?xml version="1.0" encoding="UTF-8"?>
<x><item1/><item2/><item4/><item5/></x>
--- Modern test remove index 1 at index 2 ---
bool(true)
int(0)
string(5) "item1"
bool(true)
int(1)
string(5) "item2"
bool(true)
int(2)
string(5) "item3"
bool(true)
int(3)
string(5) "item5"
<?xml version="1.0" encoding="UTF-8"?>
<x><item1/><item3/><item4/><item5/></x>
--- Modern test remove all ---
bool(true)
int(0)
string(5) "item1"
bool(true)
int(1)
string(5) "item3"
bool(true)
int(2)
string(5) "item5"
<?xml version="1.0" encoding="UTF-8"?>
<x><item2/><item4/></x>
