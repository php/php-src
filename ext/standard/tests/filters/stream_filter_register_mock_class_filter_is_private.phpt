--TEST--
stream_filter_register() with a class name exist that mocks php_user_filter with a private filter method
--FILE--
<?php

class foo {
	public $filtername;
	public $params;

	private function filter($in, $out, &$consumed, bool $closing): int {
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
resource(%d) of type (stream filter)

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d

Fatal error: Uncaught Error: Invalid callback foo::filter, cannot access private method foo::filter() in %s:%d
Stack trace:
#0 %s(%d): fwrite(Resource id #%d, 'Hello\n')
#1 {main}
  thrown in %s on line %d

Fatal error: Invalid callback foo::filter, cannot access private method foo::filter() in Unknown on line 0
