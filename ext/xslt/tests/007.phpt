--TEST--
xslt_set_opt function and public entities
--SKIPIF--
<?php
include("skipif.inc");
if(!function_exists('xslt_setopt')) {
	die("skip\n");
}
?>
--FILE--
<?php
error_reporting(E_ALL);
$xmlfile = 'ext/xslt/tests/public.xml';
$xslfile = 'ext/xslt/tests/args.xsl';

$xh = xslt_create();
// Tell Sablotron to process public entities
xslt_setopt($xh, XSLT_SAB_PARSE_PUBLIC_ENTITIES);

$result = xslt_process($xh, $xmlfile, $xslfile);
print "$result\n";

$xslstring = implode('', file($xslfile));
$xslstring = str_replace('method="text"', 'method="html"', $xslstring);
$xslstring = str_replace('<xsl:value-of select="." />', '<html><head><title>foo</title></head><body><p><xsl:value-of select="." /></p></body></html>', $xslstring);
// DEBUG: print $xslstring;

xslt_setopt($xh, XSLT_SAB_PARSE_PUBLIC_ENTITIES | XSLT_SAB_DISABLE_ADDING_META);
$result_nometa = xslt_process($xh, $xmlfile, 'arg:/_xsl', NULL, array('/_xsl' => $xslstring));
// DEBUG: print "$result_nometa\n";

xslt_setopt($xh, XSLT_SAB_PARSE_PUBLIC_ENTITIES);
$result_meta = xslt_process($xh, $xmlfile, 'arg:/_xsl', NULL, array('/_xsl' => $xslstring));
// DEBUG: print "$result_meta\n";

/* Also test if they're equal. That would mean, that disable-adding-meta is set to off
   at compile time and our call to xslt_setopt failed to reset that */
if($result_meta != $result_nometa && FALSE === stristr($result_nometa, '<meta http-equiv="Content-Type"'))
{
	print "OK\n";
}

xslt_free($xh);
?>
--EXPECT--
PHP QA®
OK
