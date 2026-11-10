--TEST--
DOMXPath: a php:function() callback cannot modify the document that is being evaluated through SimpleXML
--EXTENSIONS--
dom
simplexml
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<r a="1"><a>1<b/>2</a><a>3<b/>4</a></r>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function cb() {
    $sxe = simplexml_import_dom($GLOBALS['doc']);
    $mutations = [
        'assign' => function () use ($sxe) { $sxe->a[1] = 'z'; },
        'unset' => function () use ($sxe) { unset($sxe->a[0]); },
        'attribute' => function () use ($sxe) { $sxe['a'] = '2'; },
        'addChild' => function () use ($sxe) { $sxe->addChild('c'); },
        'addAttribute' => function () use ($sxe) { $sxe->addAttribute('d', '3'); },
    ];
    foreach ($mutations as $label => $mutation) {
        try {
            $mutation();
        } catch (Error $e) {
            $GLOBALS['errors'][$label . ": " . $e::class . ": " . $e->getMessage()] = true;
        }
    }
    return true;
}

$errors = [];
var_dump(count($xp->query('(//a/node())[php:function("cb")]')));
echo implode("\n", array_keys($errors)), "\n";
echo $doc->saveXML($doc->documentElement), "\n";

$sxe = simplexml_import_dom($doc);
$sxe->a[1] = 'z';
echo $doc->saveXML($doc->documentElement), "\n";
?>
--EXPECT--
int(6)
assign: Error: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
unset: Error: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
attribute: Error: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
addChild: Error: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
addAttribute: Error: Cannot modify a node while an XPath evaluation or XSLT transformation that uses it is in progress
<r a="1"><a>1<b/>2</a><a>3<b/>4</a></r>
<r a="1"><a>1<b/>2</a><a>z</a></r>
