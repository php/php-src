--TEST--
stream_filter_register() with a class that coerces the $consumed parameter of filter method
--SKIPIF--
<?php

// this leaks memory
// remove SKIPIF section when GH-20058 is merged OR when debug
// builds report "warn: XFAIL section but test passes"

if (getenv('SKIP_MSAN')) {
	die('skip requires a leak detector');
}

if (getenv('SKIP_ASAN') && PHP_OS_FAMILY !== 'Windows') {
	die('xfail User filters leak unprocessed buckets');
}

if (PHP_DEBUG && getenv('USE_ZEND_ALLOC') !== '0') {
	die('xfail User filters leak unprocessed buckets');
}

if (getenv('USE_ZEND_ALLOC') === '0' && getenv('ZEND_DONT_UNLOAD_MODULES') === '1') {
	die('xleak User filters leak unprocessed buckets');
}

die('skip requires a leak detector');
?>
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
