--TEST--
User filters leaving buckets behind on the input or output brigade must not leak them
--FILE--
<?php

class pass_on_without_consuming extends php_user_filter {
	public function filter($in, $out, &$consumed, bool $closing): int {
		return PSFS_PASS_ON;
	}
}

class feed_me_with_output extends php_user_filter {
	public function filter($in, $out, &$consumed, bool $closing): int {
		while ($bucket = stream_bucket_make_writeable($in)) {
			stream_bucket_append($out, $bucket);
		}
		return PSFS_FEED_ME;
	}
}

stream_filter_register("pass_on_without_consuming", "pass_on_without_consuming");
stream_filter_register("feed_me_with_output", "feed_me_with_output");

echo "Read filter, PSFS_PASS_ON without consuming input:\n";
$fp = fopen("php://memory", "w+");
fwrite($fp, "Hello");
rewind($fp);
stream_filter_append($fp, "pass_on_without_consuming", STREAM_FILTER_READ);
var_dump(fread($fp, 10));
fclose($fp);

echo "Write filter, PSFS_PASS_ON without consuming input:\n";
$fp = fopen("php://memory", "w+");
stream_filter_append($fp, "pass_on_without_consuming", STREAM_FILTER_WRITE);
var_dump(fwrite($fp, "Hello"));
fclose($fp);

echo "Write filter, PSFS_FEED_ME with buckets on output:\n";
$fp = fopen("php://memory", "w+");
stream_filter_append($fp, "feed_me_with_output", STREAM_FILTER_WRITE);
var_dump(fwrite($fp, "Hello"));
fclose($fp);

?>
--EXPECTF--
Read filter, PSFS_PASS_ON without consuming input:

Warning: fread(): Unprocessed filter buckets remaining on input brigade in %s on line %d
string(0) ""
Write filter, PSFS_PASS_ON without consuming input:

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
int(0)
Write filter, PSFS_FEED_ME with buckets on output:
int(0)
