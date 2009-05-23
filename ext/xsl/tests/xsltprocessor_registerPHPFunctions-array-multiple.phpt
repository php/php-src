--TEST--
Check xsltprocessor::registerPHPFunctions with array called multiple times
--DESCRIPTION--
When being called multiple times with an array,
registerPHPFunctions adds the new functions to the allowed parameter
list - it does not replace the previously allowed functions.
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
$phpfuncxsl->load(dirname(__FILE__)."/phpfunc.xsl");
if(!$phpfuncxsl) {
  echo "Error while parsing the xsl document\n";
  exit;
}
$proc->importStylesheet($phpfuncxsl);
var_dump($proc->registerPHPFunctions(array('strpos', 'ucwords')));
var_dump($proc->registerPHPFunctions(array('strrev', 'array_key_exists')));
var_dump($proc->registerPHPFunctions(array()));
var_dump($proc->transformToXml($dom));
--EXPECTF--
NULL
NULL
NULL
string(18) "This Is An Example"
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
