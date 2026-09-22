--TEST--
GH-17037 (UAF in user filter when adding existing filter name due to incorrect error handling)
--FILE--
<?php

class filter_string_toupper {}
try {
	var_dump(stream_filter_register('string.toupper', 'filter_string_toupper'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
bool(false)
