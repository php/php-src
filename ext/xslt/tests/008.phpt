--TEST--
xslt_set_object function
--SKIPIF--
<?php
include("skipif.inc");
if(!function_exists('xslt_set_object')) {
	die("skip\n");
}
?>
--FILE--
<?php
error_reporting(E_ALL);
class XSLTTester
{
	var $_success = false;
	var $_success2 = false;

	function XSLTTester()
	{}
	
	// this function will register this object as the
	// callback object.
	function test1($xmlfile,$xslfile)
	{
		$xh = xslt_create();
		xslt_set_object($xh,$this);
		$handlers = array('get_all'=> 'handle_getall');
		xslt_set_scheme_handlers($xh,$handlers);
		$res = xslt_process($xh,$xmlfile,$xslfile);
		xslt_free($xh);
		return 1;
	}

	// this function will pass this object as in set_scheme_handler
	function test2($xmlfile,$xslfile)
	{
		$xh = xslt_create();
		$handlers = array('get_all'=> array(&$this,'handle_getall2'));
		xslt_set_scheme_handlers($xh,$handlers);
		$res = xslt_process($xh,$xmlfile,$xslfile);
		xslt_free($xh);
		return 1;
	}
	function handle_getall($xh,$scheme,$rest)
	{
		$this->_success = true;
		$rest = substr($rest,2);
		return implode('', file('ext/xslt/tests/'.$rest));
	}
	function handle_getall2($xh,$scheme,$rest)
	{
		$this->_success2 = true;
		$rest = substr($rest,2);
		return implode('', file('ext/xslt/tests/'.$rest));
	}
	function testSucceeded()
	{
		return $this->_success;
	}
	function test2Succeeded()
	{
		return $this->_success2;
	}
}

$xmlfile = 'ext/xslt/tests/test.xml';
$xslfile = 'ext/xslt/tests/xslt_set_object.xsl';

$testobj = new XSLTTester();
$testobj->test1($xmlfile,$xslfile);

$testobj->test2($xmlfile,$xslfile);

if ($testobj->testSucceeded())
	print "OK\n";
else
	print "FAILED\n";

if ($testobj->test2Succeeded())
	print "OK\n";
else
	print "FAILED\n";
	
?>
--EXPECT--
OK
OK
