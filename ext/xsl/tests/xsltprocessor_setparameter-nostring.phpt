--TEST--
Check xsltprocessor::setparameter error handling with no-string
--DESCRIPTION--
Memleak: http://bugs.php.net/bug.php?id=48221
--SKIPIF--
<?php
if (!extension_loaded('xsl')) {
    die("skip\n");
}
?>
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$proc->importStylesheet($xsl);
try {
    $proc->setParameter('', array(4, 'abc'));
} catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
}
$proc->transformToXml($dom);
?>
--EXPECT--
XSLTProcessor::setParameter(): Argument #2 ($name) must contain only string keys
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
