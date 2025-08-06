--TEST--
IntlTimeZone::getErrorCode/Message(): basic test
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

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

var_dump($lsb->getErrorCode());
var_dump($lsb->getErrorMessage());

echo "Call to getOffset():\n";
var_dump($lsb->getOffset(INF, 1, $a, $b));

var_dump($lsb->getErrorCode());
var_dump($lsb->getErrorMessage());

?>
--EXPECT--
int(0)
string(12) "U_ZERO_ERROR"
Call to getOffset():
bool(false)
int(1)
string(75) "IntlTimeZone::getOffset(): error obtaining offset: U_ILLEGAL_ARGUMENT_ERROR"
