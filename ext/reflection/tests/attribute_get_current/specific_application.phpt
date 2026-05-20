--TEST--
ReflectionAttribute::getCurrent() returns the ReflectionAttribute for the specific usage
--FILE--
<?php

#[Attribute(Attribute::TARGET_ALL|Attribute::IS_REPEATABLE)]
class Demo {
	public function __construct($args) {
		var_dump($args);
		var_dump(ReflectionAttribute::getCurrent()->getArguments());
	}
}

#[Demo("one string param")]
#[Demo(true, 123), Demo(null)]
const GLOBAL_CONSTANT = true;

$case = new ReflectionConstant('GLOBAL_CONSTANT');
echo $case;
echo "\n";
foreach ($case->getAttributes() as $attrib) {
	var_dump($attrib->getArguments());
	$attrib->newInstance();
	echo "\n";
}

?>
--EXPECT--
Constant [ bool GLOBAL_CONSTANT ] { 1 }

array(1) {
  [0]=>
  string(16) "one string param"
}
string(16) "one string param"
array(1) {
  [0]=>
  string(16) "one string param"
}

array(2) {
  [0]=>
  bool(true)
  [1]=>
  int(123)
}
bool(true)
array(2) {
  [0]=>
  bool(true)
  [1]=>
  int(123)
}

array(1) {
  [0]=>
  NULL
}
NULL
array(1) {
  [0]=>
  NULL
}
