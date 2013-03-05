--TEST--
Bug #64353 (Built-in classes can be unavailable with dynamic includes and Optimizer+)
--INI--
zend_optimizerplus.enable=1
zend_optimizerplus.enable_cli=1
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class BugLoader extends php_user_filter {
	public function filter($in, $out, &$consumed, $closing) {
		if (!class_exists("Test")) {
			eval("class Test extends ArrayObject {}");
		}
		while ($bucket = stream_bucket_make_writeable($in)) {
			$bucket->data = strtoupper($bucket->data);
			$consumed += $bucket->datalen;
//			stream_bucket_append($out, $bucket);
		}
		return PSFS_PASS_ON;
	}
}

stream_filter_register('bug.test', 'BugLoader');
include "php://filter/read=bug.test/resource=test.php";
echo "OK\n";
?>
--EXPECT--
OK
