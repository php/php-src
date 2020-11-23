--TEST--
Bug #30702 (cannot initialize class variable from class constant)
--FILE--
<?php
class foo {
    const C1=1;
}

class bar extends foo {
  const C2=2;

  public $c1=bar::C1;
  public $c2=bar::C2;

  public $c3=self::C1;
  public $c4=self::C2;

  public $c5=foo::C1;
  public $c6=parent::C1;
}

$x= new bar();
var_dump($x);
?>
--EXPECT--
object(bar)#1 (6) {
  ["c1"]=>
  int(1)
  ["c2"]=>
  int(2)
  ["c3"]=>
  int(1)
  ["c4"]=>
  int(2)
  ["c5"]=>
  int(1)
  ["c6"]=>
  int(1)
}
