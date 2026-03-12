--TEST--
Bug #66873 - crash in UConverter with invalid encoding
--EXTENSIONS--
intl
--FILE--
<?php
try {
    $o = new UConverter(1, 1);
    $o->toUCallback(1, 1, 1, $b);
    var_dump($o->getErrorCode());
} catch (Throwable $e) {
	echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: UConverter::__construct(): returned error 4: U_FILE_ACCESS_ERROR
