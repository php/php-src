--TEST--
Check xsltprocessor::removeParameter wrong parameter handling
--SKIPIF--
<?php
        if (!extension_loaded('xsl')) {
                die("skip\n");
        }
?>
--FILE--
<?php
include dirname(__FILE__) .'/prepare.inc';
$proc->removeParameter();
$proc->removeParameter(array(), array());
$proc->removeParameter('', array());
--EXPECTF--
Warning: XSLTProcessor::removeParameter() expects exactly 2 parameters, 0 given in %s on line %d

Warning: XSLTProcessor::removeParameter() expects parameter 1 to be string, array given in %s on line %d

Warning: XSLTProcessor::removeParameter() expects parameter 2 to be string, array given in %s on line %d
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
