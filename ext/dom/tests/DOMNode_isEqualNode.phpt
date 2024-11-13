--TEST--
DOMNode::isEqualNode()
--EXTENSIONS--
dom
--SKIPIF--
<?php
if (LIBXML_VERSION >= 21200 && LIBXML_VERSION <= 21201) {
    die("xfail Broken for libxml2 2.12.0 - 2.12.1 see https://gitlab.gnome.org/GNOME/libxml2/-/issues/634");
}
?>
--FILE--
<?php

$dom1 = new DOMDocument();
$dom2 = new DOMDocument();

$dom1->loadXML(<<<XML
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd" [
    <!ENTITY bar '<bar>bartext</bar>'>
    <!ENTITY foo '<foo/>'>
    <!NOTATION myNotation SYSTEM "test.dtd">
]>
<html>
    <body>
        <pi><?test 123?><?test2 123?><?test2 1234?><?test 123?></pi>
        <ps><p>text</p><p>text</p><p>other text</p></ps>
        <psattrs><p align="center" xmlns:foo="some:bar">text</p><p xmlns:foo="some:bar">text</p><p align="center">text</p><p align="left" xmlns:foo="some:bar">text</p><p align="center" xmlns:foo="some:bar2">text</p><p align="center" xmlns:foo="some:bar">text</p></psattrs>
        <comments><!-- comment 1 --><!-- comment 2 --><!-- comment 1 --></comments>
        <texts>abc<i/>def<i/>abc</texts>
        <cdatas><![CDATA[test]]> <![CDATA[test2]]> <![CDATA[test]]></cdatas>
        <tree>
            <div>
                <p>A</p>
                <div foo="bar">
                    <p>B</p>
                </div>
            </div>
            <p>A</p>
        </tree>
    </body>
</html>
XML);

$xpath = new DOMXPath($dom1);

function foreach_comparator($query) {
    global $xpath;
    $container = $xpath->query($query)[0];
    $childNodes = iterator_to_array($container->childNodes);
    $firstChild = $childNodes[0];
    foreach ($childNodes as $child) {
        var_dump($child->isEqualNode($firstChild));
    }
}

function comparePairs($list1, $list2) {
    $list1 = iterator_to_array($list1);
    $list2 = iterator_to_array($list2);
    usort($list1, function ($a, $b) {
        return strcmp($a->nodeName, $b->nodeName);
    });
    usort($list2, function ($a, $b) {
        return strcmp($a->nodeName, $b->nodeName);
    });
    foreach ($list1 as $entity1) {
        foreach ($list2 as $entity2) {
            echo "Comparing {$entity1->nodeName} with {$entity2->nodeName}\n";
            var_dump($entity1->isEqualNode($entity2));
        }
    }
}

echo "--- Test edge cases ---\n";

var_dump($dom1->doctype->isEqualNode(null));
var_dump((new DOMDocument())->isEqualNode(new DOMDocument()));

echo "--- Test doctype ---\n";

var_dump($dom1->doctype->isEqualNode($dom1->doctype));
$dom2->loadXML('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/x.dtd"><html/>');
var_dump($dom1->doctype->isEqualNode($dom2->doctype));
$dom2->loadXML('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN2" "http://www.w3.org/TR/html4/strict.dtd"><html/>');
var_dump($dom1->doctype->isEqualNode($dom2->doctype));
$dom2->loadXML('<!DOCTYPE HTML2 PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd"><html/>');
var_dump($dom1->doctype->isEqualNode($dom2->doctype));
$dom2->loadXML('<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd"><html/>');
var_dump($dom1->doctype->isEqualNode($dom2->doctype));

echo "--- Test processing instruction ---\n";

foreach_comparator("//pi");

echo "--- Test comments ---\n";

foreach_comparator("//comments");

echo "--- Test texts ---\n";

foreach_comparator("//texts");

echo "--- Test CDATA ---\n";

foreach_comparator("//cdatas");

echo "--- Test attribute ---\n";

var_dump((new DOMAttr("name", "value"))->isEqualNode(new DOMAttr("name", "value")));
var_dump((new DOMAttr("name", "value"))->isEqualNode(new DOMAttr("name", "value2")));
var_dump((new DOMAttr("name", "value"))->isEqualNode(new DOMAttr("name2", "value")));
var_dump((new DOMAttr("name", "value"))->isEqualNode(new DOMAttr("name2", "value2")));
var_dump((new DOMAttr("name", "value"))->isEqualNode(new DOMAttr("ns:name", "value")));

echo "--- Test entity reference ---\n";

var_dump((new DOMEntityReference("ref"))->isEqualNode(new DOMEntityReference("ref")));
var_dump((new DOMEntityReference("ref"))->isEqualNode(new DOMEntityReference("ref2")));

echo "--- Test entity declaration ---\n";

$dom2->loadXML(<<<XML
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd" [
    <!ENTITY barbar '<bar>bartext</bar>'>
    <!ENTITY foo '<foo2/>'>
    <!ENTITY bar '<bar>bartext</bar>'>
]>
<html/>
XML);

comparePairs($dom1->doctype->entities, $dom2->doctype->entities);

echo "--- Test notation declaration ---\n";

$dom2->loadXML(<<<XML
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd" [
    <!NOTATION myNotation SYSTEM "test.dtd">
    <!NOTATION myNotation2 SYSTEM "test2.dtd">
    <!NOTATION myNotation3 SYSTEM "test.dtd">
]>
<html/>
XML);

comparePairs($dom1->doctype->notations, $dom2->doctype->notations);

echo "--- Test element without attributes ---\n";

foreach_comparator("//ps");

echo "--- Test element with attributes ---\n";

foreach_comparator("//psattrs");

echo "--- Test element tree ---\n";

$tree = $xpath->query("//tree")[0];
$dom2->loadXML(<<<XML
        <tree>
            <div>
                <p>A</p>
                <div foo="bar">
                    <p>B</p>
                </div>
            </div>
            <p>A</p>
        </tree>
XML);
var_dump($tree->isEqualNode($dom2->documentElement));
$dom2->loadXML(<<<XML
        <tree>
            <p>A</p>
            <div>
                <p>A</p>
                <div foo="bar">
                    <p>B</p>
                </div>
            </div>
        </tree>
XML);
var_dump($tree->isEqualNode($dom2->documentElement));
$dom2->loadXML(<<<XML
        <tree>
            <div>
                <p>A</p>
                <div foo="bar">
                </div>
            </div>
            <p>A</p>
        </tree>
XML);
var_dump($tree->isEqualNode($dom2->documentElement));
$dom2->loadXML(<<<XML
        <tree>
            <div>
                <p>A</p>
                <div foo="bar" extra="attr">
                    <p>B</p>
                </div>
            </div>
            <p>A</p>
        </tree>
XML);
var_dump($tree->isEqualNode($dom2->documentElement));

echo "--- Test documents ---\n";

$dom1Clone = clone $dom1;
var_dump($dom1->documentElement->isEqualNode($dom1Clone->documentElement));
var_dump($dom1->documentElement->isEqualNode($dom2->documentElement));
var_dump($dom1->isEqualNode($dom1Clone));
var_dump($dom1->isEqualNode($dom2));
var_dump($dom1->documentElement->isEqualNode($dom1Clone));
var_dump($dom1->documentElement->isEqualNode($dom2));

echo "--- Test document fragments ---\n";

$fragment1 = $dom1->createDocumentFragment();
$fragment1->appendChild($dom1->createElement('em'));
$fragment2 = $dom1->createDocumentFragment();
$fragment2->appendChild($dom1->createElement('em'));
$fragment3 = $dom1->createDocumentFragment();
$fragment3->appendChild($dom1->createElement('b'));
$emptyFragment1 = $dom1->createDocumentFragment();
$emptyFragment2 = $dom1->createDocumentFragment();

var_dump($fragment1->isEqualNode($fragment2));
var_dump($fragment1->isEqualNode($fragment3));
var_dump($emptyFragment1->isEqualNode($fragment1));
var_dump($emptyFragment1->isEqualNode($emptyFragment2));

echo "--- Test document fragments with multiple child nodes ---\n";

$fragment1 = $dom1->createDocumentFragment();
$fragment1->appendChild($dom1->createElement('a'));
$fragment1->appendChild($dom1->createElement('b'));
$fragment1->appendChild($dom1->createElement('c'));
$fragment2 = $dom2->createDocumentFragment();
$fragment2->appendChild($dom2->createElement('a'));
$fragment2->appendChild($dom2->createElement('b'));
$fragment2->appendChild($dom2->createElement('c'));

var_dump($fragment1->isEqualNode($fragment2));

$fragment2->firstChild->nextSibling->nextSibling->remove();
var_dump($fragment1->isEqualNode($fragment2));

echo "--- Test x:includes ---\n";

// Adapted from https://www.php.net/manual/en/domdocument.xinclude.php
$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0" ?>
<chapter xmlns:xi="http://www.w3.org/2001/XInclude">
 <p>Hello</p>
 <para>
  <xi:include href="book.xml">
   <xi:fallback>
    <p>xinclude: book.xml not found</p>
   </xi:fallback>
  </xi:include>
 </para>
 <para><p>xinclude: book.xml not found</p></para>
</chapter>
XML);
@$dom->xinclude();
$xpath = new DOMXPath($dom);

$firstPara = $dom->documentElement->firstElementChild->nextElementSibling;
$secondPara = $dom->documentElement->firstElementChild->nextElementSibling->nextElementSibling;
var_dump($firstPara->isEqualNode($secondPara));
var_dump($firstPara->firstElementChild->isEqualNode($secondPara->firstElementChild));

?>
--EXPECT--
--- Test edge cases ---
bool(false)
bool(true)
--- Test doctype ---
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
--- Test processing instruction ---
bool(true)
bool(false)
bool(false)
bool(true)
--- Test comments ---
bool(true)
bool(false)
bool(true)
--- Test texts ---
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
--- Test CDATA ---
bool(true)
bool(false)
bool(false)
bool(false)
bool(true)
--- Test attribute ---
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
--- Test entity reference ---
bool(true)
bool(false)
--- Test entity declaration ---
Comparing bar with bar
bool(true)
Comparing bar with barbar
bool(false)
Comparing bar with foo
bool(false)
Comparing foo with bar
bool(false)
Comparing foo with barbar
bool(false)
Comparing foo with foo
bool(true)
--- Test notation declaration ---
Comparing myNotation with myNotation
bool(true)
Comparing myNotation with myNotation2
bool(false)
Comparing myNotation with myNotation3
bool(false)
--- Test element without attributes ---
bool(true)
bool(true)
bool(false)
--- Test element with attributes ---
bool(true)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
--- Test element tree ---
bool(true)
bool(false)
bool(false)
bool(false)
--- Test documents ---
bool(true)
bool(false)
bool(true)
bool(false)
bool(false)
bool(false)
--- Test document fragments ---
bool(true)
bool(false)
bool(false)
bool(true)
--- Test document fragments with multiple child nodes ---
bool(true)
bool(false)
--- Test x:includes ---
bool(false)
bool(true)
