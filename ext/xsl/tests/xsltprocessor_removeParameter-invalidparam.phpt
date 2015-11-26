--TEST--
Check xsltprocessor::removeParameter with invalid parameter
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
var_dump($proc->removeParameter('', 'doesnotexist'));
--EXPECT--
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
