--TEST--
Doctrine-like attributes usage
--FILE--
<?php
namespace Doctrine\ORM {

	class Entity {
		private $name;
		public function __construct($name) {
			$this->name = $name;
		}
	}

	function GetClassAttributes($class_name) {
		$reflClass = new \ReflectionClass($class_name);
		$attrs = $reflClass->getAttributes();
		foreach ($attrs as $name => &$values) {
			$name = "Doctrine\\" . $name;
			$values = new $name(...$values);
		}
		return $attrs;
	}
}

namespace {
	<<ORM\Entity("user")>>
	class User {}

	var_dump(Doctrine\ORM\GetClassAttributes("User"));
}
?>
--EXPECT--
array(1) {
  ["ORM\Entity"]=>
  object(Doctrine\ORM\Entity)#2 (1) {
    ["name":"Doctrine\ORM\Entity":private]=>
    string(4) "user"
  }
}
