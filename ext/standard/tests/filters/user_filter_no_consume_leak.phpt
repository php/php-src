--TEST--
User filter that does not consume any input bucket leaks
--FILE--
<?php

class noop_filter extends php_user_filter {
	public function filter($in, $out, &$consumed, bool $closing): int {
		return PSFS_PASS_ON;
	}
}

stream_filter_register("noop_filter", "noop_filter");

stream_filter_append(STDOUT, "noop_filter");

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
int(0)
