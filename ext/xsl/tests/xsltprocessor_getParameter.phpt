--TEST--
Check xsltprocessor::getparameter functionality
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
$proc->setParameter('', 'key', 'value');
var_dump($proc->getParameter('', 'key'));
?>
--EXPECT--
string(5) "value"
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
