--TEST--
Do not leak when promoting null offset deprecation
--FILE--
<?php

set_error_handler(function ($errno, $errstr) {
    throw new Exception($errstr);
});

try {
	$a = ['foo' => 'bar', null => new stdClass];
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Exception: Using null as an array offset is deprecated, use an empty string instead
