--TEST--
Don't override xslt_set_base (bug #20518)
--SKIPIF--
<?php // vim600: syn=php ts=4 sw=4
include("skipif.inc");
mkdir(dirname(__FILE__) . '/tmp', 0777) or die("skip Cannot create working directory");
?>
--INI--
magic_quotes_runtime=0
--FILE--
<?php
error_reporting(E_ALL);
copy(dirname(__FILE__) . '/qa.dtd', dirname(__FILE__) . '/tmp/qa.dtd');
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
xslt_set_base($xh, 'file://' . dirname(__FILE__) . '/tmp/');
$result = xslt_process($xh, 'arg:/_xml', 'arg:/_xsl', NULL, array('/_xml' => $xmldata, '/_xsl' => $xslsheet));
print "$result\n";
xslt_free($xh);
unlink(dirname(__FILE__) . '/tmp/qa.dtd');
rmdir(dirname(__FILE__) . '/tmp');
?>
--EXPECT--
Test has passed
