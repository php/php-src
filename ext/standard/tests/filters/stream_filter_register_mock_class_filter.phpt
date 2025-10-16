--TEST--
stream_filter_register() with a class name exist that mocks php_user_filter with a filter method
--XFAIL--
This leaks memory
--FILE--
<?php

class foo {
	public $filtername;
	public $params;

	public function filter($in, $out, &$consumed, bool $closing): int {
		return PSFS_PASS_ON;
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
