--TEST--
extractTo
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . 'test_with_comment.zip';
include $dirname . 'utils.inc';
$zip = new ZipArchive;
if ($zip->open($file) !== TRUE) {
	echo "open failed.\n";
	exit('failed');
}

$zip->extractTo($dirname . '__oo_extract_tmp');
if (!is_dir($dirname . '__oo_extract_tmp')) {
	echo "failed. mkdir\n";
}

if (!is_dir($dirname .'__oo_extract_tmp/foobar')) {
	echo "failed. mkdir foobar\n";
}

if (!file_exists($dirname . '__oo_extract_tmp/foobar/baz')) {
	echo "failed. extract foobar/baz\n";
} else {
	echo file_get_contents($dirname . '__oo_extract_tmp/foobar/baz') . "\n";
}

if (!file_exists($dirname . '__oo_extract_tmp/bar')) {
	echo "failed. bar file\n";
} else {
	echo file_get_contents($dirname . '__oo_extract_tmp/bar') . "\n";
}

if (!file_exists($dirname . '__oo_extract_tmp/foo')) {
	echo "failed. foo file\n";
} else {
	echo file_get_contents($dirname . '__oo_extract_tmp/foo') . "\n";
}


/* extract one file */
$zip->extractTo($dirname . '__oo_extract_tmp', 'bar');
if (!file_exists($dirname . '__oo_extract_tmp/bar')) {
	echo "failed. extract  bar file\n";
} else {
	echo file_get_contents($dirname . '__oo_extract_tmp/bar') . "\n";
}

/* extract two files */
$zip->extractTo($dirname . '__oo_extract_tmp', array('bar','foo'));
if (!file_exists($dirname . '__oo_extract_tmp/bar')) {
	echo "failed. extract  bar file\n";
} else {
	echo file_get_contents($dirname . '__oo_extract_tmp/bar') . "\n";
}
if (!file_exists($dirname . '__oo_extract_tmp/foo')) {
	echo "failed. extract foo file\n";
} else {
	echo file_get_contents($dirname . '__oo_extract_tmp/foo') . "\n";
}

rmdir_rf($dirname . '__oo_extract_tmp');
?>
--EXPECT--
blabla laber rababer sülz

bar

foo


bar

bar

foo
