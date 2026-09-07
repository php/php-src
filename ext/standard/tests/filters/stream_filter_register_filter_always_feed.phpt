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

?>
--EXPECTF--
bool(true)
resource(4) of type (stream filter)

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
int(0)
