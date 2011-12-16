--TEST--
Private (relevant to #60536)
--FILE--
<?php
error_reporting(E_ALL | E_STRICT);

class BaseWithPropA {
  private $hello = 0;
}

trait AHelloProperty {
  private $hello = 0;
}

class BaseWithTPropB {
    use AHelloProperty;
}

class SubclassA extends BaseWithPropA {
    use AHelloProperty;
}

class SubclassB extends BaseWithTPropB {
    use AHelloProperty;
}

$a = new SubclassA;
var_dump($a);

$b = new SubclassB;
var_dump($b);

?>
--EXPECTF--	
Strict Standards: BaseWithPropA and AHelloProperty define the same property ($hello) in the composition of SubclassA. This might be incompatible, to improve maintainability consider using accessor methods in traits instead. Class was composed in %sbug60536_003.php on line %d

Strict Standards: BaseWithTPropB and AHelloProperty define the same property ($hello) in the composition of SubclassB. This might be incompatible, to improve maintainability consider using accessor methods in traits instead. Class was composed in %sbug60536_003.php on line %d
object(SubclassA)#%d (2) {
  ["hello":"SubclassA":private]=>
  int(0)
  ["hello":"BaseWithPropA":private]=>
  int(0)
}
object(SubclassB)#%d (2) {
  ["hello":"SubclassB":private]=>
  int(0)
  ["hello":"BaseWithTPropB":private]=>
  int(0)
}
