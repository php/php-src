--TEST--
Reflection inheritance
--FILE--
<?php

class ReflectionClassEx extends ReflectionClass
{
	public $bla;

	function getMethodNames()
	{
		$res = array();
		foreach($this->getMethods() as $m)
		{
			$res[] = $m->class . '::' . $m->name;
		}
		return $res;
	}
}

$r = new ReflectionClassEx('ReflectionClassEx');

$exp = array (
  'UMLClass::__clone',
  'UMLClass::export',
  'UMLClass::__construct',
  'UMLClass::__toString',
  'UMLClass::getName',
  'UMLClass::isInternal',
  'UMLClass::isUserDefined',
  'UMLClass::isInstantiable',
  'UMLClass::getFileName',
  'UMLClass::getStartLine',
  'UMLClass::getEndLine',
  'UMLClass::getDocComment',
  'UMLClass::getConstructor',
  'UMLClass::getMethod',
  'UMLClass::getMethods',
  'UMLClass::getProperty',
  'UMLClass::getProperties',
  'UMLClass::getConstants',
  'UMLClass::getConstant',
  'UMLClass::getInterfaces',
  'UMLClass::isInterface',
  'UMLClass::isAbstract',
  'UMLClass::isFinal',
  'UMLClass::getModifiers',
  'UMLClass::isInstance',
  'UMLClass::newInstance',
  'UMLClass::getParentClass',
  'UMLClass::isSubclassOf',
  'UMLClass::getStaticProperties',
  'UMLClass::getDefaultProperties',
  'UMLClass::isIterateable',
  'UMLClass::implementsInterface',
  'UMLClass::getExtension',
  'UMLClass::getExtensionName');

$miss = array();

$res = $r->getMethodNames();

foreach($exp as $m)
{
	if (!in_array($m, $exp))
	{
		$miss[] = $m;
	}
}

var_dump($miss);

$props = array_keys(get_class_vars('ReflectionClassEx'));
sort($props);
var_dump($props);
var_dump($r->name);
?>
===DONE===
--EXPECT--
array(0) {
}
array(2) {
  [0]=>
  string(3) "bla"
  [1]=>
  string(4) "name"
}
string(17) "ReflectionClassEx"
===DONE===
