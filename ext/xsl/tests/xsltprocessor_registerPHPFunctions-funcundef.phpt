--TEST--
Check xsltprocessor::registerPHPFunctions and a undefined php function
--DESCRIPTION--
The XSL script tries to call a php function that is not defined
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
$phpfuncxsl->load(dirname(__FILE__)."/phpfunc-undef.xsl");
if(!$phpfuncxsl) {
  echo "Error while parsing the xsl document\n";
  exit;
}
$proc->importStylesheet($phpfuncxsl);
var_dump($proc->registerPHPFunctions());
var_dump($proc->transformToXml($dom));
--EXPECTF--
NULL

Warning: XSLTProcessor::transformToXml(): Unable to call handler undefinedfunc() in %s on line %d
NULL
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
