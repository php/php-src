--TEST--
GH-15837 (Segmentation fault in ext/simplexml/simplexml.c)
--CREDITS--
YuanchengJiang
--FILE--
<?php
$xml =<<<EOF
<xml>
<fieldset1>
</fieldset1>
<fieldset2>
<options>
</options>
</fieldset2>
</xml>
EOF;
$sxe = new SimpleXMLIterator($xml);
$rit = new RecursiveIteratorIterator($sxe, RecursiveIteratorIterator::LEAVES_ONLY);
foreach ($rit as $child) {
    $ancestry = $child->xpath('ancestor-or-self::*');
    // Exhaust internal iterator
    foreach ($ancestry as $ancestor) {
    }
}
var_dump($rit->valid());
var_dump($rit->key());
?>
--EXPECT--
bool(false)
NULL
