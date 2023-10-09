--TEST--
Bug #67440 (append_node of a DOMDocumentFragment does not reconcile namespaces)
--EXTENSIONS--
dom
--FILE--
<?php

function createDocument() {
    $document = new DOMDocument();
    $document->loadXML('<?xml version="1.0"?><rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2"></rootElement>');
    $fragment = $document->createDocumentFragment();
    $fragment->appendChild($document->createTextNode("\n"));
    $fragment->appendChild($document->createElementNS('http://example/ns', 'myns:childNode', '1'));
    $fragment->appendChild($document->createTextNode("\n"));
    $fragment->appendChild($document->createElementNS('http://example/ns', 'myns:childNode', '2'));
    $fragment->appendChild($document->createTextNode("\n"));
    return array($document, $fragment);
}

function case1($method) {
    list($document, $fragment) = createDocument();
    $document->documentElement->{$method}($fragment);
    echo $document->saveXML();
}

function case2($method) {
    list($document, $fragment) = createDocument();
    $childNodes = iterator_to_array($fragment->childNodes);
    foreach ($childNodes as $childNode) {
        $document->documentElement->{$method}($childNode);
    }
    echo $document->saveXML();
}

function case3($method) {
    list($document, $fragment) = createDocument();
    $fragment->removeChild($fragment->firstChild);
    $document->documentElement->{$method}($fragment);
    echo $document->saveXML();
}

function case4($method) {
    list($document, $fragment) = createDocument();
    $fragment->childNodes[1]->appendChild($document->createElementNS('http://example/ns2', 'myns2:childNode', '3'));
    $document->documentElement->{$method}($fragment);
    echo $document->saveXML();
}

echo "== appendChild ==\n";
echo "-- fragment to document element --\n"; case1('appendChild'); echo "\n";
echo "-- children manually document element --\n"; case2('appendChild'); echo "\n";
echo "-- fragment to document where first element is not a text node --\n"; case3('appendChild'); echo "\n";
echo "-- fragment with namespace declarations in children --\n"; case4('appendChild'); echo "\n";

echo "== insertBefore ==\n";
echo "-- fragment to document element --\n"; case1('insertBefore'); echo "\n";
echo "-- children manually document element --\n"; case2('insertBefore'); echo "\n";
echo "-- fragment to document where first element is not a text node --\n"; case3('insertBefore'); echo "\n";
echo "-- fragment with namespace declarations in children --\n"; case4('insertBefore'); echo "\n";

echo "== insertAfter ==\n";
echo "-- fragment to document element --\n"; case1('insertBefore'); echo "\n";
echo "-- children manually document element --\n"; case2('insertBefore'); echo "\n";
echo "-- fragment to document where first element is not a text node --\n"; case3('insertBefore'); echo "\n";
echo "-- fragment with namespace declarations in children --\n"; case4('insertBefore'); echo "\n";

?>
--EXPECT--
== appendChild ==
-- fragment to document element --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- children manually document element --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- fragment to document where first element is not a text node --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2"><myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- fragment with namespace declarations in children --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1<myns2:childNode>3</myns2:childNode></myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

== insertBefore ==
-- fragment to document element --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- children manually document element --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- fragment to document where first element is not a text node --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2"><myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- fragment with namespace declarations in children --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1<myns2:childNode>3</myns2:childNode></myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

== insertAfter ==
-- fragment to document element --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- children manually document element --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- fragment to document where first element is not a text node --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2"><myns:childNode>1</myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>

-- fragment with namespace declarations in children --
<?xml version="1.0"?>
<rootElement xmlns:myns="http://example/ns" xmlns:myns2="http://example/ns2">
<myns:childNode>1<myns2:childNode>3</myns2:childNode></myns:childNode>
<myns:childNode>2</myns:childNode>
</rootElement>
