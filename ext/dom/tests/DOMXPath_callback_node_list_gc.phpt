--TEST--
DOMXPath callback node list is reported to the cycle collector
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument();
$doc->loadXML('<r><a/></r>');
$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');
$xp->registerPhpFunctions();

function cb($n) {
    @$n[0]->back = $GLOBALS['the_xp'];
    return true;
}

$GLOBALS['the_xp'] = $xp;
$wr = WeakReference::create($xp);
$xp->query('/r/a[php:function("cb", .)]');

unset($xp, $GLOBALS['the_xp']);
gc_collect_cycles();

var_dump($wr->get() === null);
?>
--EXPECT--
bool(true)
