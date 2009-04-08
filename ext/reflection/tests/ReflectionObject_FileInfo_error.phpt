--TEST--
ReflectionObject::getFileName(), ReflectionObject::getStartLine(), ReflectionObject::getEndLine() -invalid aparams 
--FILE-- 
<?php
Class C { }

$rc = new ReflectionObject(new C);
$methods = array("getFileName", "getStartLine", "getEndLine");

foreach ($methods as $method) {
	var_dump($rc->$method());
	var_dump($rc->$method(null));
	var_dump($rc->$method('X', 0));	
}
?>
--EXPECTF--
string(%d) "%s"

Warning: Wrong parameter count for ReflectionClass::getFileName() in %s on line 9
NULL

Warning: Wrong parameter count for ReflectionClass::getFileName() in %s on line 10
NULL
int(2)

Warning: Wrong parameter count for ReflectionClass::getStartLine() in %s on line 9
NULL

Warning: Wrong parameter count for ReflectionClass::getStartLine() in %s on line 10
NULL
int(2)

Warning: Wrong parameter count for ReflectionClass::getEndLine() in %s on line 9
NULL

Warning: Wrong parameter count for ReflectionClass::getEndLine() in %s on line 10
NULL
