--TEST--
Relative and absolute arg handling
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
$xslfile = dirname(__FILE__) . '/args.xsl';
$xslsheet = @implode('', @file($xslfile));
$xmldata = '<?xml version="1.0" encoding="ISO-8859-1"?>';
$xmldata .= '<!DOCTYPE qa SYSTEM "qa.dtd">' . "\n";
$xmldata .='
<qa>
	<test type="simple">Test has passed</test>
	<test type="complex" />
</qa>';

$xh = xslt_create();
$result = xslt_process($xh, 'arg:/_xml', $xslfile, NULL, array('/_xml' => $xmldata));
print "$result\n";
$result = xslt_process($xh, 'arg:/_xml', 'arg:/_xsl', NULL, array('/_xml' => $xmldata, '/_xsl' => $xslsheet));
print "$result\n";
xslt_free($xh);
?>
--EXPECT--
Test has passed
Test has passed
