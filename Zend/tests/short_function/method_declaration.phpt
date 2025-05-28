--TEST--
Short function method declaration
--FILE--
<?php
class Decorator {
  public $proxy;
  function getId() => $this->proxy->id;
  function getName() => $this->proxy->name;

  function setId($value) => $this->proxy->id = $value;
  function setName($value) => $this->proxy->name = $value;
}

$decorated = new stdClass;
$decorated->id = null;
$decorated->name = null;

$decorator = new Decorator;
$decorator->proxy = $decorated;

var_dump($decorated);

$decorator->setId(1);
$decorator->setName('Dmitrii');

var_dump($decorated);
?>
--EXPECT--
object(stdClass)#1 (2) {
  ["id"]=>
  NULL
  ["name"]=>
  NULL
}
object(stdClass)#1 (2) {
  ["id"]=>
  int(1)
  ["name"]=>
  string(7) "Dmitrii"
}