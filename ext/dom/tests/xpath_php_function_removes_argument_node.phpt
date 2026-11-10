--TEST--
DOMXPath: a php:function callback cannot remove its argument node mid-evaluation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root><a>1</a><a>2</a><a>3</a><a>4</a></root>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function cb($nodes) {
    foreach ($nodes as $n) {
        try {
            $n->parentNode->removeChild($n);
        } catch (DOMException $e) {
            echo $e::class, ": ", $e->getMessage(), "\n";
        }
    }
    return true;
}

$res = $xp->query('//a[php:function("cb", .)]');
foreach ($res as $r) {
    var_dump($r->nodeName);
}
echo "done\n";
?>
--EXPECT--
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
string(1) "a"
string(1) "a"
string(1) "a"
string(1) "a"
done
