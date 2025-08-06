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
ini_set("intl.error_level", E_WARNING);

$tz = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($tz->getOffset(INF, true, $a, $a));

?>
--EXPECTF--
Warning: IntlTimeZone::getOffset(): error obtaining offset in %s on line %d
bool(false)
