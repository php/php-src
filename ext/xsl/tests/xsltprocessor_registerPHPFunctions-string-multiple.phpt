--TEST--
Check xsltprocessor::registerPHPFunctions with string called multiple times
--DESCRIPTION--
When being called multiple times with a stringular function name only,
registerPHPFunctions adds the new function to the allowed parameter
list - it does not replace the old function.
--SKIPIF--
<?php
        if (!extension_loaded('xsl')) {
                die("skip\n");
        }
?>
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$phpfuncxsl = new domDocument();
$phpfuncxsl->load(__DIR__."/phpfunc.xsl");
if(!$phpfuncxsl) {
  echo "Error while parsing the xsl document\n";
  exit;
}
$proc->importStylesheet($phpfuncxsl);
var_dump($proc->registerPHPFunctions('ucwords'));
var_dump($proc->registerPHPFunctions('strpos'));
var_dump($proc->transformToXml($dom));
?>
--EXPECT--
NULL
NULL
string(18) "This Is An Example"
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
