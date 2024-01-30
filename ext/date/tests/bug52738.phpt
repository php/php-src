--TEST--
Bug #52738 (Can't use new properties in class extended from DateInterval)
--FILE--
<?php
class di extends DateInterval {
    public $unit = 1;
}

$I = new di('P10D');
echo $I->unit."\n";
$I->unit++;
echo $I->unit."\n";
$I->unit = 42;
echo $I->unit."\n";
$I->d++;
var_dump($I);
?>
--EXPECTF--
1
2
42
object(di)#%d (%d) {
  ["from_string"]=>
  bool(false)
  ["unit"]=>
  int(42)
  ["y"]=>
  int(0)
  ["m"]=>
  int(0)
  ["d"]=>
  int(11)
  ["h"]=>
  int(0)
  ["i"]=>
  int(0)
  ["s"]=>
  int(0)
  ["f"]=>
  float(0)
  ["invert"]=>
  int(0)
  ["days"]=>
  bool(false)
}
