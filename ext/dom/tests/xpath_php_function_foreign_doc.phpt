--TEST--
DOMXPath: php:function nodeset args plus a foreign-document return must not treat arrays as DOM objects
--EXTENSIONS--
dom
--FILE--
<?php
$doc1 = new DOMDocument();
$doc1->loadXML('<root><a>1</a></root>');
$doc2 = new DOMDocument();
$doc2->loadXML('<root><b>2</b></root>');

$xp = new DOMXPath($doc1);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function uses_nodeset($nodes) {
    return true;
}

function foreign() {
    global $doc2;
    return $doc2->documentElement;
}

$xp->query('//a[php:function("uses_nodeset", //a)]');
$res = $xp->query('php:function("foreign")');
echo "count: ";
var_dump($res->length);
$n = $res->item(0);
echo "name: ";
var_dump($n->nodeName);
echo "owner is doc2: ";
var_dump($n->ownerDocument === $doc2);
echo "done\n";
?>
--EXPECT--
count: int(1)
name: string(4) "root"
owner is doc2: bool(true)
done
