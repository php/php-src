--TEST--
Handling of public fields with traits needs to have same semantics as with normal inheritance, however, we do add strict warnings since it is easier to run into something unexpeted with changing traits.
--FILE--
<?php
error_reporting(E_ALL | E_STRICT);

class BaseWithPropA {
  public $hello = 0;
}

// This is how publics are handled in normal inheritance
class SubclassClassicInheritance extends BaseWithPropA {
  public $hello = 0;
}

// And here, we need to make sure, that the traits behave the same

trait AHelloProperty {
  public $hello = 0;
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

$classic = new SubclassClassicInheritance;
var_dump($classic);

$a = new SubclassA;
var_dump($a);

$b = new SubclassB;
var_dump($b);

?>
--EXPECTF--

object(SubclassClassicInheritance)#1 (1) {
  ["hello"]=>
  int(0)
}
object(SubclassA)#2 (1) {
  ["hello"]=>
  int(0)
}
object(SubclassB)#3 (1) {
  ["hello"]=>
  int(0)
}