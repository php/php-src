--TEST--
Check xsltprocessor::getparameter error handling
--SKIPIF--
<?php 
        if (!extension_loaded('xsl')) {
                die("skip\n");
        }
?>
--FILE--
<?php
include dirname(__FILE__) .'/prepare.inc';
var_dump($proc->getParameter());
var_dump($proc->getParameter(array(), array()));
var_dump($proc->getParameter('', array()));
--EXPECTF--
Warning: XSLTProcessor::getParameter() expects exactly 2 parameters, 0 given in %s on line %d
bool(false)

Warning: XSLTProcessor::getParameter() expects parameter 1 to be %binary_string_optional%, array given in %s on line %d
bool(false)

Warning: XSLTProcessor::getParameter() expects parameter 2 to be %binary_string_optional%, array given in %s on line %d
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
