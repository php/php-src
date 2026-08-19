--TEST--
stream_filter_register() with a filter method always returning PSFS_FEED_ME
--FILE--
<?php
class foo extends php_user_filter {
	public function filter($in, $out, &$consumed, bool $closing): int {
		return PSFS_FEED_ME;
	}
}

var_dump(stream_filter_register("invalid_filter", "foo"));

var_dump(stream_filter_append(STDOUT, "invalid_filter"));

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

$stream = fopen('data://text/plain,Hello', 'r');
var_dump(stream_filter_append($stream, "invalid_filter"));
var_dump(stream_get_contents($stream));

?>
--EXPECTF--
bool(true)
resource(4) of type (stream filter)

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
int(0)
resource(%d) of type (stream filter)

Warning: stream_get_contents(): Unprocessed filter buckets remaining on input brigade in %s on line %d
string(0) ""
