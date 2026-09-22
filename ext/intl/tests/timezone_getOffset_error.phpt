--TEST--
IntlTimeZone::getOffset(): errors
--EXTENSIONS--
intl
--SKIPIF--
<?php
/* INF being an invalid offset depends on UB in float->int cast behavior. */
$arch = php_uname('m');
if ($arch != 'x86_64' && $arch != 'i386')
    die('skip requires x86');
?>
--FILE--
<?php

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getOffset(INF, true, $a, $a));
echo intl_get_error_message(), PHP_EOL;

?>
--EXPECT--
bool(false)
IntlTimeZone::getOffset(): error obtaining offset: U_ILLEGAL_ARGUMENT_ERROR
