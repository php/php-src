--TEST--
DOMXPath: a php:function() callback cannot modify a document whose node another callback returned
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<root/>');
$other = new DOMDocument();
$other->loadXML('<other><a>1</a><a>2</a><a>3</a></other>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function other_root() {
    return $GLOBALS['other']->documentElement;
}

function cb() {
    $root = $GLOBALS['other']->documentElement;
    try {
        foreach (iterator_to_array($root->childNodes) as $child) {
            $root->removeChild($child);
        }
    } catch (DOMException $e) {
        $GLOBALS['errors'][$e::class . ": " . $e->getMessage()] = true;
    }
    return true;
}

$errors = [];
$res = $xp->evaluate('php:function("other_root")/a[php:function("cb")]');
echo "count: ", count($res), "\n";
echo implode("\n", array_keys($errors)), "\n";
echo $other->saveXML($other->documentElement), "\n";

$other->documentElement->removeChild($other->documentElement->firstChild);
echo $other->saveXML($other->documentElement), "\n";
?>
--EXPECT--
count: 3
DOMException: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<other><a>1</a><a>2</a><a>3</a></other>
<other><a>2</a><a>3</a></other>
