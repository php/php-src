--TEST--
Check xsltprocessor::registerPHPFunctions and a undefined php function
--DESCRIPTION--
The XSL script tries to call a php function that is not defined
--EXTENSIONS--
xsl
--FILE--
<?php
include __DIR__ .'/prepare.inc';
$phpfuncxsl = new domDocument();
$phpfuncxsl->load(__DIR__."/phpfunc-undef.xsl");
if(!$phpfuncxsl) {
  echo "Error while parsing the xsl document\n";
  exit;
}
$proc->importStylesheet($phpfuncxsl);
var_dump($proc->registerPHPFunctions());
try {
  var_dump($proc->transformToXml($dom));
} catch (Throwable $e) {
  echo $e->getMessage(), "\n";
}
?>
--EXPECT--
NULL
Invalid callback undefinedfunc, function "undefinedfunc" not found or invalid function name
--CREDITS--
Christian Weiske, cweiske@php.net
PHP Testfest Berlin 2009-05-09
