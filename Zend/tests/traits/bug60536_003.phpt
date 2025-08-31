--TEST--
Properties should be initialized correctly (relevant to #60536)
--FILE--
<?php

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
object(SubclassA)#%d (2) {
  ["hello":"BaseWithPropA":private]=>
  int(0)
  ["hello":"SubclassA":private]=>
  int(0)
}
object(SubclassB)#%d (2) {
  ["hello":"BaseWithTPropB":private]=>
  int(0)
  ["hello":"SubclassB":private]=>
  int(0)
}
