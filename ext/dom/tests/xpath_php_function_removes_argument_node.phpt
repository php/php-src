--TEST--
DOMXPath: a php:function callback that removes its argument node must not free it mid-evaluation
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
        if ($n->parentNode) {
            $n->parentNode->removeChild($n);
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
string(1) "a"
string(1) "a"
string(1) "a"
string(1) "a"
done
