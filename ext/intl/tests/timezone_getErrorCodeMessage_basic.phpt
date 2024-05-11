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
ini_set("intl.error_level", E_WARNING);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');

var_dump($lsb->getErrorCode());
var_dump($lsb->getErrorMessage());

var_dump($lsb->getOffset(INF, 1, $a, $b));

var_dump($lsb->getErrorCode());
var_dump($lsb->getErrorMessage());

?>
--EXPECTF--
int(0)
string(12) "U_ZERO_ERROR"

Warning: IntlTimeZone::getOffset(): error obtaining offset in %s on line %d
bool(false)
int(1)
string(48) "error obtaining offset: U_ILLEGAL_ARGUMENT_ERROR"
