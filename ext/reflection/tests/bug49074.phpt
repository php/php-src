--TEST--
Bug #49074 (private class static fields can be modified by using reflection)
--FILE--
<?php
class Test {
	private static $data1 = 1;
	private static $data4 = 4;
}

class Test2 extends Test {
	private static $data2 = 2;
	public static $data3 = 3;
}

$r = new ReflectionClass('Test2');
$m = $r->getStaticProperties();

$m['data1'] = 100;
$m['data2'] = 200;
$m['data3'] = 300;
$m['data4'] = 400;

var_dump($r->getStaticProperties());
?>
--EXPECT--
array(4) {
  [u"data2"]=>
  int(2)
  [u"data3"]=>
  int(3)
  [u"data1"]=>
  int(1)
  [u"data4"]=>
  int(4)
}
