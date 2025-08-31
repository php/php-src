--TEST--
Bug #62081: IntlDateFormatter leaks memory if called twice
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
$x = new IntlDateFormatter('en', 1, 1);
try {
    var_dump($x->__construct('en', 1, 1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
IntlException: IntlDateFormatter::__construct(): cannot call constructor twice
