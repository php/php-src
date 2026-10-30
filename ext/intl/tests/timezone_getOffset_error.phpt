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

intltz_get_offset(null, time()*1000, false, $a, $a);
?>
--EXPECTF--
Warning: IntlTimeZone::getOffset(): error obtaining offset in %s on line %d
bool(false)

Fatal error: Uncaught TypeError: intltz_get_offset(): Argument #1 ($timezone) must be of type IntlTimeZone, null given in %s:%d
Stack trace:
#0 %s(%d): intltz_get_offset(NULL, %f, false, NULL, NULL)
#1 {main}
  thrown in %s on line %d
