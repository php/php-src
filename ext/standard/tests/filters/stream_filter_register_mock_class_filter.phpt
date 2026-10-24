--TEST--
stream_filter_register() with a class name exist that mocks php_user_filter with a filter method
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
