--TEST--
stream_filter_register() with a class name exist that mocks php_user_filter with a filter method returning not an int
--FILE--
<?php

class foo {
	public $filtername;
	public $params;

	public function filter($in, $out, &$consumed, bool $closing) {
		return new stdClass();
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
int(0)

Warning: Object of class stdClass could not be converted to int in Unknown on line 0
