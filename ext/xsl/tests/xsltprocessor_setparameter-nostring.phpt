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
include dirname(__FILE__) .'/prepare.inc';
$proc->importStylesheet($xsl);
var_dump($proc->setParameter('', array(4, 'abc')));
$proc->transformToXml($dom);
--EXPECTF--
Warning: XSLTProcessor::setParameter(): Invalid parameter array in %s on line %d
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
