--TEST--
Various ways to provide xml and xslt arguments and params
--SKIPIF--
<?php
include("skipif.inc");
if(!function_exists('utf8_encode')) {
	die("skip function utf8_encode() not available");
}
?>
--INI--
magic_quotes_runtime=0
--FILE--
<?php
error_reporting(E_ALL);
$xmlfile = 'ext/xslt/tests/test.xml';
$xslfile = 'ext/xslt/tests/args.xsl';
$xmldata = @implode('', @file($xmlfile));
$xslsheet = @implode('', @file($xslfile));

$xh = xslt_create();
$result = xslt_process($xh, $xmlfile, $xslfile);
print "$result\n";
$result = xslt_process($xh, 'arg:/_xml', $xslfile, NULL, array('/_xml' => $xmldata));
print "$result\n";
$result = xslt_process($xh, $xmlfile, 'arg:/_xsl', NULL, array('/_xsl' => $xslsheet));
print "$result\n";
$result = xslt_process($xh, 'arg:/_xml', 'arg:/_xsl', NULL, array('/_xml' => $xmldata, '/_xsl' => $xslsheet));
print "$result\n";

// The same, with params
$xslfile = 'ext/xslt/tests/param.xsl';
$xslsheet = implode('', file($xslfile));
$params = array("Test has passed", "PHP QA®");

foreach($params AS $val)
{
	$val = utf8_encode($val);
	$result = xslt_process($xh, $xmlfile, $xslfile, NULL, NULL, array('insertion' => $val));
	print "$result\n";
	$result = xslt_process($xh, 'arg:/_xml', $xslfile, NULL, array('/_xml' => $xmldata), array('insertion' => $val));
	print "$result\n";
	$result = xslt_process($xh, $xmlfile, 'arg:/_xsl', NULL, array('/_xsl' => $xslsheet), array('insertion' => $val));
	print "$result\n";
	$result = xslt_process($xh, 'arg:/_xml', 'arg:/_xsl', NULL, array('/_xml' => $xmldata, '/_xsl' => $xslsheet), array('insertion' => $val));
	print "$result\n";
}

xslt_free($xh);
?>
--EXPECT--
Test has passed
Test has passed
Test has passed
Test has passed
Test has passed
Test has passed
Test has passed
Test has passed
PHP QA®
PHP QA®
PHP QA®
PHP QA®
