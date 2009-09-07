--TEST--
Check xsltprocessor::registerPHPFunctions and a non-string function in xsl
--DESCRIPTION--
The XSL script tries to call a php function that is not a string which
is expected to fail
--SKIPIF--
<?php 
        if (!extension_loaded('xsl')) {
                die("skip\n");
        }
?>
--FILE--
<?php
include dirname(__FILE__) .'/prepare.inc';
$phpfuncxsl = new domDocument();
$phpfuncxsl->load(dirname(__FILE__)."/phpfunc-nostring.xsl");
if(!$phpfuncxsl) {
  echo "Error while parsing the xsl document\n";
  exit;
}
$proc->importStylesheet($phpfuncxsl);
var_dump($proc->registerPHPFunctions());
var_dump($proc->transformToXml($dom));
--EXPECTF--
NULL

Warning: XSLTProcessor::transformToXml(): Handler name must be a string in %s on line %d

Warning: XSLTProcessor::transformToXml(): xmlXPathCompiledEval: evaluation failed in %s on line %d

Warning: XSLTProcessor::transformToXml(): runtime error: file %s line %d element value-of in %s on line %d

Warning: XSLTProcessor::transformToXml(): xsltValueOf: text copy failed in %s on line %d
bool(false)
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
