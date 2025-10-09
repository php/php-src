--TEST--
stream_filter_register() with a class that coerces the $consumed parameter of filter method
--XFAIL--
This leaks memory
--FILE--
<?php

class foo extends php_user_filter {
	public function filter($in, $out, &$consumed, bool $closing): int {
		$consumed = new stdClass();
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

Warning: Object of class stdClass could not be converted to int in %s on line %d

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
int(1)

Warning: Object of class stdClass could not be converted to int in Unknown on line 0
