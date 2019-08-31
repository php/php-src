--TEST--
Bug #53603 (ZipArchive should quiet stat errors)
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php

class TestStream {
	function url_stat($path, $flags) {
		if (!($flags & STREAM_URL_STAT_QUIET))
			trigger_error("not quiet");
		return array();
	}
}

stream_wrapper_register("teststream", "TestStream");

$dirname = __DIR__ . '/';
$file = $dirname . 'test_with_comment.zip';
$zip = new ZipArchive;
if ($zip->open($file) !== TRUE) {
	echo "open failed.\n";
	exit('failed');
}

$a = $zip->extractTo('teststream://test');
var_dump($a);
--EXPECTF--
Warning: ZipArchive::extractTo(teststream://test/foo): failed to open stream: "TestStream::stream_open" call failed in %s on line %d
bool(false)
