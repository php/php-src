--TEST--
xslt_setopt function and public entities
--SKIPIF--
<?php
include("skipif.inc");
if(!function_exists('xslt_setopt')) {
	die("skip function xslt_setopt() is not available\n");
}
?>
--INI--
magic_quotes_runtime=0
--FILE--
<?php
error_reporting(E_ALL);
$xmlfile = dirname(__FILE__).'/public.xml';
$xslfile = dirname(__FILE__).'/args.xsl';

$xh = xslt_create();
// Tell Sablotron to process public entities
xslt_setopt($xh, XSLT_SABOPT_PARSE_PUBLIC_ENTITIES);

$result = xslt_process($xh, $xmlfile, $xslfile);
print "$result\n";

$xslstring = implode('', file($xslfile));
$xslstring = str_replace('method="text"', 'method="html"', $xslstring);
$xslstring = str_replace('<xsl:value-of select="." />', '<html><head><title>foo</title></head><body><p><xsl:value-of select="." /></p></body></html>', $xslstring);
// DEBUG: print $xslstring;

xslt_setopt($xh, XSLT_SABOPT_PARSE_PUBLIC_ENTITIES | XSLT_SABOPT_DISABLE_ADDING_META);
var_dump(xslt_process($xh, $xmlfile, 'arg:/_xsl', NULL, array('/_xsl' => $xslstring)));


xslt_setopt($xh, XSLT_SABOPT_PARSE_PUBLIC_ENTITIES);
var_dump(xslt_process($xh, $xmlfile, 'arg:/_xsl', NULL, array('/_xsl' => $xslstring)));
// DEBUG: print "$result_meta\n";

xslt_free($xh);
?>
--EXPECT--
PHP QA®
string(95) "<html>
  <head>
    <title>foo</title>
  </head>
  <body>
    <p>PHP QA®</p>
  </body>
</html>
"
string(172) "<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=ISO-8859-1">
    <title>foo</title>
  </head>
  <body>
    <p>PHP QA®</p>
  </body>
</html>
"
