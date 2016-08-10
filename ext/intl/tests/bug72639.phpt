--TEST--
Bug #72639 (Segfault when instantiating class that extends IntlCalendar and adds a property)
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
class A extends IntlCalendar {
    public function __construct() {}
	private $a;
}
var_dump(new A());
?>
--EXPECTF--
object(A)#%d (1) {
  ["valid"]=>
  bool(false)
}
