--TEST--
Pass long string to 'file' argument, bug #17791
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php
$xmlstring = str_repeat('x', 512);
$xslstring = 'x';
$xh = xslt_create();
$result = @xslt_process($xh, $xmlstring, $xslstring);
@xslt_free($xh);
echo("OK");
?>
--EXPECT--
OK
