--TEST--
Bug #39551 (Segfault with stream_bucket_new in user filter)
--FILE--
<?php

$bucket = stream_bucket_new(fopen('php://temp', 'w+'), '');
var_dump($bucket);

class bucketFilter extends php_user_filter {
    public function filter($in, $out, &$consumed, $closing ): int {

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
object(StreamBucket)#%d (%d) {
  ["bucket"]=>
  resource(%d) of type (userfilter.bucket)
  ["data"]=>
  string(0) ""
  ["datalen"]=>
  int(0)
  ["dataLength"]=>
  int(0)
}
Done
