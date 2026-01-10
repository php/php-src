--TEST--
GH-11500 (Namespace reuse in createElementNS() generates wrong output)
--EXTENSIONS--
dom
--FILE--
<?php
function api_test_depth2($root_ns) {
    $dom = new DOMDocument();
    $root = $dom->createElementNS($root_ns, 'root');
    $dom->appendChild($root);

    $a1 = $dom->createElementNS('http://example.com', 'a1');
    $b1 = $a1->appendChild($dom->createElementNS('http://example.com', 'b1'));
    $root->appendChild($a1);

    $a2 = $dom->createElementNS('http://example.com', 'a2');
    $b2 = $a2->appendChild($dom->createElementNS('http://example.com', 'b2'));
    $root->appendChild($a2);

    echo $dom->saveXML();

    var_dump($root->namespaceURI);
    var_dump($a1->namespaceURI);
    var_dump($b1->namespaceURI);
    var_dump($a2->namespaceURI);
    var_dump($b2->namespaceURI);
}

function api_test_depth3($root_ns, $swapped) {
    $dom = new DOMDocument();
    $root = $dom->createElementNS($root_ns, 'root');
    $dom->appendChild($root);

    $a1 = $dom->createElementNS('http://example.com', 'a1');
    $b1 = $a1->appendChild($dom->createElementNS('http://example.com', 'b1'));
    $c1 = $b1->appendChild($dom->createElementNS('http://example.com', 'c1'));
    $root->appendChild($a1);

    $a2 = $dom->createElementNS('http://example.com', 'a2');
    if ($swapped) {
        $b2 = $dom->createElementNS('http://example.com', 'b2');
        $c2 = $b2->appendChild($dom->createElementNS('http://example.com', 'c2'));
        $a2->appendChild($b2);
    } else {
        $b2 = $a2->appendChild($dom->createElementNS('http://example.com', 'b2'));
        $c2 = $b2->appendChild($dom->createElementNS('http://example.com', 'c2'));
    }
    $root->appendChild($a2);

    echo $dom->saveXML();

    var_dump($root->namespaceURI);
    var_dump($a1->namespaceURI);
    var_dump($b1->namespaceURI);
    var_dump($c1->namespaceURI);
    var_dump($a2->namespaceURI);
    var_dump($b2->namespaceURI);
    var_dump($c2->namespaceURI);
}

echo "-- Constructed from API (depth 2, mismatched root variation) --\n";
api_test_depth2('http://example2.com');

echo "-- Constructed from API (depth 2, matching root variation) --\n";
api_test_depth2('http://example.com');

echo "-- Constructed from API (depth 3, mismatched root variation, non-swapped) --\n";
api_test_depth3('http://example2.com', false);

echo "-- Constructed from API (depth 3, matching root variation, non-swapped) --\n";
api_test_depth3('http://example.com', false);

echo "-- Constructed from API (depth 3, mismatched root variation, swapped) --\n";
api_test_depth3('http://example2.com', true);

echo "-- Constructed from API (depth 3, matching root variation, swapped) --\n";
api_test_depth3('http://example.com', true);

echo "-- Constructed depth 2 from string --\n";
$xml = '<?xml version="1.0"?><root xmlns="http://example2.com"><a1 xmlns="http://example.com"><b1/></a1><a2 xmlns="http://example.com"><b2/></a2></root>';
$dom = new DOMDocument;
$dom->loadXML($xml);
echo $dom->saveXML(), "\n";

var_dump($dom->documentElement->namespaceURI); // root
var_dump($dom->documentElement->firstChild->namespaceURI); // a1
var_dump($dom->documentElement->firstChild->firstChild->namespaceURI); // b1
var_dump($dom->documentElement->firstChild->nextSibling->namespaceURI); // a2
var_dump($dom->documentElement->firstChild->nextSibling->firstChild->namespaceURI); // b2
?>
--EXPECT--
-- Constructed from API (depth 2, mismatched root variation) --
<?xml version="1.0"?>
<root xmlns="http://example2.com"><a1 xmlns="http://example.com"><b1/></a1><a2 xmlns="http://example.com"><b2/></a2></root>
string(19) "http://example2.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
-- Constructed from API (depth 2, matching root variation) --
<?xml version="1.0"?>
<root xmlns="http://example.com"><a1><b1/></a1><a2><b2/></a2></root>
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
-- Constructed from API (depth 3, mismatched root variation, non-swapped) --
<?xml version="1.0"?>
<root xmlns="http://example2.com"><a1 xmlns="http://example.com"><b1><c1/></b1></a1><a2 xmlns="http://example.com"><b2><c2/></b2></a2></root>
string(19) "http://example2.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
-- Constructed from API (depth 3, matching root variation, non-swapped) --
<?xml version="1.0"?>
<root xmlns="http://example.com"><a1><b1><c1/></b1></a1><a2><b2><c2/></b2></a2></root>
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
-- Constructed from API (depth 3, mismatched root variation, swapped) --
<?xml version="1.0"?>
<root xmlns="http://example2.com"><a1 xmlns="http://example.com"><b1><c1/></b1></a1><a2 xmlns="http://example.com"><b2><c2/></b2></a2></root>
string(19) "http://example2.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
-- Constructed from API (depth 3, matching root variation, swapped) --
<?xml version="1.0"?>
<root xmlns="http://example.com"><a1><b1><c1/></b1></a1><a2><b2><c2/></b2></a2></root>
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
-- Constructed depth 2 from string --
<?xml version="1.0"?>
<root xmlns="http://example2.com"><a1 xmlns="http://example.com"><b1/></a1><a2 xmlns="http://example.com"><b2/></a2></root>

string(19) "http://example2.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
string(18) "http://example.com"
