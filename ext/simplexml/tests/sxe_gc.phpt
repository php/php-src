--TEST--
SimpleXMLElement reports its temporary and iterator values to the cycle collector
--EXTENSIONS--
simplexml
--FILE--
<?php

function collected(WeakReference $w): bool
{
    gc_collect_cycles();
    return $w->get() === null;
}

$sxe = new SimpleXMLElement('<root/>');
$w = WeakReference::create($sxe);
$r =& $sxe->missing;
$r = $sxe;
unset($r, $sxe);
echo "missing property reference: ";
var_dump(collected($w));

$sxe = new SimpleXMLElement('<root><a/></root>');
$w = WeakReference::create($sxe);
$sxe->rewind();
$current = $sxe->current();
$r =& $current->missing;
$r = $sxe;
unset($r, $current, $sxe);
echo "current element: ";
var_dump(collected($w));

$sxe = new SimpleXMLElement('<root><a><b/></a></root>');
$w = WeakReference::create($sxe);
$sxe->rewind();
$child = $sxe->current();
$child->rewind();
$grandchild = $child->current();
$r =& $grandchild->missing;
$r = $sxe;
unset($r, $grandchild, $child, $sxe);
echo "nested current element: ";
var_dump(collected($w));

function iterate(SimpleXMLElement $sxe): Generator
{
    foreach ($sxe as $element) {
        unset($sxe, $element);
        yield;
    }
}

$sxe = new SimpleXMLElement('<root><a/></root>');
$w = WeakReference::create($sxe);
$r =& $sxe->missing;
$gen = iterate($sxe);
$gen->current();
$r = $gen;
unset($r, $gen, $sxe);
echo "foreach iterator in generator: ";
var_dump(collected($w));

?>
--EXPECT--
missing property reference: bool(true)
current element: bool(true)
nested current element: bool(true)
foreach iterator in generator: bool(true)
