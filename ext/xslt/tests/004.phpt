--TEST--
Set a non-existing scheme handler
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
function bar()
{
	return 1;
}
$xh = xslt_create();
$xmlstring = '<foo><barred /></foo>';
$xslstring = '<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/TR/1999/XSL/Transform"><xsl:for-each select="/"><xsl:value-of select="document(\'bogus://foo\')" /></xsl:for-each></xsl:stylesheet>';
xslt_set_scheme_handlers($xh, array('get_all' => 'foo'));
$result = xslt_process($xh, 'arg:/_xml', 'arg:/_xsl', NULL, array('/_xml' => $xmlstring, '/_xsl' => $xslstring));
xslt_free($xh);
echo("OK");
?>
--EXPECT--
OK
