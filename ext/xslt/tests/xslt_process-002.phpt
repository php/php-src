--TEST--
Crash xslt_process with reused handler (this test may take a while)
--SKIPIF--
<?php
include("skipif.inc");
if(!function_exists('utf8_encode')) {
	die("skip\n");
}
?>
--FILE--
<?php
error_reporting(E_ALL);
$xmlfile = 'ext/xslt/tests/test.xml';
$xslfile = 'ext/xslt/tests/param.xsl';
$xmldata = @implode('', @file($xmlfile));
$xslsheet = @implode('', @file($xslfile));

/*
 * Tested on a Cyrix 200MMX/128MB took 2 secs. Should be a reasonable margin.
 *
 * It's not meant as an actual speed test, but if it's slower than this,
 * there must be something significantly off in the php/sablot/expat trio.
 * Emulation OS's come to mind...
 */
$want_time = 6;

function make_param()
{
	$ret_val = '';
	$numchars = mt_rand(2,16);
	$illegal = array(0,256,512);
	for($i=0;$i<$numchars;$i++)
	{
		$char=0;
		while(in_array($char, $illegal))
		{
			$char .= mt_rand(32, 512);
		}
		$ret_val .= chr($char);
	}

	return utf8_encode($ret_val);
}

function decode($string)
{
	$ret_val = '';
	for($i=0; $i<strlen($string);$i++)
	{
		$ret_val .= ord(substr($string,$i,1)) . " ";
	}
	return $ret_val;
}


$xh = xslt_create();

$t1 = time();
for ($i=0; $i<50; $i++)
{
	$val = make_param();
	$result = xslt_process($xh, $xmlfile, $xslfile, NULL, NULL, array('insertion' => $val));
	if(!$result or $result != utf8_decode($val))
		print "Failed $i / ".utf8_decode($val).": $result\n\tDecode: " . decode(utf8_decode($val)) . "\n" ;
}
print "OK\n";
xslt_free($xh);
$t2 = time();
$op_time = $t2 - $t1;
if($op_time > $want_time)
	print "This test took more than $want_time seconds. Either you have a very slow / busy machine, or there's something very wrong with the speed. Your time: $op_time";
?>
--EXPECT--
OK
