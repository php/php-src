--TEST--
Bug #66921 - Wrong argument type hint for function intltz_from_date_time_zone
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$f = new ReflectionFunction('intltz_from_date_time_zone');
var_dump($f->getParameters()[0]->getClass());

?>
--EXPECTF--
object(ReflectionClass)#%d (1) {
  ["name"]=>
  string(12) "DateTimeZone"
}
