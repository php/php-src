--TEST--
DOMXPath: detached subtrees and returned nodes stay alive while a php:function() evaluation uses them
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root/>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function subtree() {
    $tree = $GLOBALS['doc']->createElement('tree');
    foreach (['a', 'b', 'c'] as $name) {
        $tree->appendChild($GLOBALS['doc']->createElement($name));
    }
    return $tree;
}

function drop() {
    unset($GLOBALS['tree']);
    return true;
}

function first_of_tree() {
    return $GLOBALS['tree']->firstChild;
}

function reconstruct() {
    try {
        $GLOBALS['tree']->__construct('other');
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    return true;
}

$tree = subtree();
$context = $tree->firstChild;
foreach ($xp->query('../*[php:function("drop")]', $context) as $node) {
    echo $node->nodeName, "\n";
}

$tree = subtree();
foreach ($xp->query('php:function("first_of_tree")/../*[php:function("drop")]') as $node) {
    echo $node->nodeName, "\n";
}

$tree = subtree();
foreach ($xp->query('*[php:function("reconstruct")]', $tree) as $node) {
    echo $node->nodeName, "\n";
}

class Unconstructed extends DOMElement {
    public function __construct() {}
}

function unconstructed() {
    return new Unconstructed();
}

var_dump($xp->evaluate('count(php:function("unconstructed"))'));

$docless = new DOMElement('docless', 'abc');

function docless() {
    return $GLOBALS['docless'];
}

function modify_docless() {
    try {
        $GLOBALS['docless']->textContent = 'xyz';
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    return true;
}

foreach ($xp->query('php:function("docless")/text()[php:function("modify_docless")]') as $node) {
    var_dump($node->nodeValue);
}
$docless->textContent = 'xyz';
var_dump($docless->textContent);

$doctype = (new DOMImplementation())->createDocumentType('q');

function doctype() {
    return $GLOBALS['doctype'];
}

function attach_doctype() {
    try {
        (new DOMImplementation())->createDocument('', 'r', $GLOBALS['doctype']);
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
    return 1;
}

var_dump($xp->evaluate('count(php:function("doctype")) + php:function("attach_doctype")'));
var_dump($doctype->ownerDocument);
?>
--EXPECT--
a
b
c
a
b
c
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
a
b
c
float(0)
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
string(3) "abc"
string(3) "xyz"
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
float(2)
NULL
