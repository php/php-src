--TEST--
Memoryleak in error printing
--SKIPIF--
<?php include("skipif.inc"); ?>
--INI--
error_reporting=2047
display_errors=0
track_errors=1
--FILE--
<?php
$xmlfile = 'ext/xslt/tests/non-existent.xml';
$xslfile = 'ext/xslt/tests/non-existent.xsl';

$xh = xslt_create();
$result = xslt_process($xh, $xmlfile, $xslfile, NULL);
xslt_free($xh);

echo $php_errormsg."\n";
?>
--EXPECTF--
Sablotron error on line none: cannot open file '%s/ext/xslt/tests/non-existent.xsl'
