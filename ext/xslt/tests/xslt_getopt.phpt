--TEST--
xslt_getopt function
--SKIPIF--
<?php
include("skipif.inc");
if(!function_exists('xslt_getopt')) {
	die("skip\n");
}
?>
--FILE--
<?php
error_reporting(E_ALL);

$xh = xslt_create();
xslt_setopt($xh, XSLT_SABOPT_PARSE_PUBLIC_ENTITIES);
if(xslt_getopt($xh) == XSLT_SABOPT_PARSE_PUBLIC_ENTITIES)
	print("OK\n");
else
	var_dump(xslt_getopt($xh));

xslt_setopt($xh, XSLT_SABOPT_PARSE_PUBLIC_ENTITIES | XSLT_SABOPT_DISABLE_ADDING_META);
if(xslt_getopt($xh) == XSLT_SABOPT_PARSE_PUBLIC_ENTITIES | XSLT_SABOPT_DISABLE_ADDING_META)
	print("OK\n");
else
	var_dump(xslt_getopt($xh));

xslt_setopt($xh, xslt_getopt($xh) | XSLT_OPT_SILENT);
if(xslt_getopt($xh) == XSLT_SABOPT_PARSE_PUBLIC_ENTITIES | XSLT_SABOPT_DISABLE_ADDING_META | XSLT_OPT_SILENT)
	print("OK\n");
else
	var_dump(xslt_getopt($xh));
xslt_free($xh);
?>
--EXPECT--
OK
OK
OK
