--TEST--
stream_filter_register() with a class whose input brigade is modified by a warning handler
--SKIPIF--
<?php

// this leaks memory
// remove SKIPIF section when debug builds report "warn: XFAIL section but test passes"

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

class ReentrantFilter extends php_user_filter
{
	private bool $firstCall = true;

	public function filter($in, $out, &$consumed, bool $closing): int
	{
		if ($this->firstCall) {
			$this->firstCall = false;
			$GLOBALS['brigade'] = $in;
			$GLOBALS['bucket'] = stream_bucket_new($this->stream, 'refilled');
		}

		return PSFS_PASS_ON;
	}
}

stream_filter_register('test.reentrant', ReentrantFilter::class);

set_error_handler(static function (int $severity, string $message): bool
{
	if (str_contains($message, 'Unprocessed filter buckets')) {
		stream_bucket_append($GLOBALS['brigade'], $GLOBALS['bucket']);
		echo "Handled warning\n";
		return true;
	}

	return false;
});

$stream = fopen('php://memory', 'w+');
stream_filter_append($stream, 'test.reentrant', STREAM_FILTER_WRITE);

var_dump(fwrite($stream, 'input'));
fclose($stream);

?>
--EXPECT--
Handled warning
int(0)
