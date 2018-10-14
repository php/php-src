--TEST--
Bug #39551 (Segfault with stream_bucket_new in user filter)
--FILE--
<?php

$bucket = stream_bucket_new(fopen('php://temp', 'w+'), '');

class bucketFilter {
	public function filter($in, $out, &$consumed, $closing ){

		$bucket = stream_bucket_new(fopen('php://temp', 'w+'), '');
		stream_bucket_append($out, $bucket);
		return PSFS_PASS_ON;
	}
}

stream_filter_register('bucketfault', 'bucketFilter');
stream_filter_append($s = fopen('php://temp', 'r+'), 'bucketfault');
stream_get_contents($s);

echo "Done\n";
?>
--EXPECTF--
Done
