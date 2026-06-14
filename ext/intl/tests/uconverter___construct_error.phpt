--TEST--
Basic UConverter::convert() usage
--EXTENSIONS--
intl
--FILE--
<?php
try {
	$c = new UConverter("\x80", 'utf-8');
	var_dump($c);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
	$c = new UConverter('utf-8', "\x80");
	var_dump($c);
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: UConverter::__construct(): returned error 4: U_FILE_ACCESS_ERROR
IntlException: UConverter::__construct(): returned error 4: U_FILE_ACCESS_ERROR
