--TEST--
Memoryleak in error printing
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
error_reporting(E_ALL);
$xmlfile = 'ext/xslt/tests/non-existent.xml';
$xslfile = 'ext/xslt/tests/non-existent.xsl';

$xh = xslt_create();
$result = xslt_process($xh, $xmlfile, $xslfile, NULL);
xslt_free($xh);
?>
--EXPECTF--
Warning: Sablotron error on line none: cannot open file '%s/ext/xslt/tests/non-existent.xsl' in %s/ext/xslt/tests/009.%s on line %i

%sext/xslt/tests/009.%s(%i) : Warning - Sablotron error on line none: cannot open file '%sext/xslt/tests/non-existent.xsl'
