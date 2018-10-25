--TEST--
Test png2wbmp() function : wrong origin filename param
--CREDITS--
Levi Fukumori <levi [at] fukumori [dot] com [dot] br>
#testfest PHPSP on 2009-06-20
--SKIPIF--
<?php
if(!extension_loaded('gd')) {
    die('skip gd extension is not loaded');
}
if(!function_exists('png2wbmp')) {
    die('skip png2wbmp function is not available');
}
?>
--FILE--
<?php
$file = dirname(__FILE__) .'/simpletext.wbmp';
png2wbmp('', $file, 20, 120, 8);
png2wbmp(null, $file, 20, 120, 8);
png2wbmp(false, $file, 20, 120, 8);
?>
--EXPECTF--
Deprecated: Function png2wbmp() is deprecated in %s on line %d

Warning: png2wbmp(): Unable to open '' for reading in %s on line %d

Deprecated: Function png2wbmp() is deprecated in %s on line %d

Warning: png2wbmp(): Unable to open '' for reading in %s on line %d

Deprecated: Function png2wbmp() is deprecated in %s on line %d

Warning: png2wbmp(): Unable to open '' for reading in %s on line %d
--CLEAN--
<?php
unlink(dirname(__FILE__) .'/simpletext.wbmp');
?>
