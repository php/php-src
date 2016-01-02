--TEST--
ZE3 __toInt()
--FILE--
<?php

function my_error_handler($errno, $errstr, $errfile, $errline) {
	var_dump($errstr);
}

set_error_handler('my_error_handler');

class TestEmptyClass
{
}

class TestToIntClass
{
    function __toInt()
    {
		echo __METHOD__ . "()\n";
        return 42;
    }
}

class TestToIntReturnsString
{
    function __toString()
    {
		echo __METHOD__ . "()\n";
        return "Foo";
    }
}

echo "====test1====\n";
$o = new TestEmptyClass;
print_r($o);
var_dump((int) $o);
var_dump($o);

echo "====test2====\n";
$o = new TestToIntClass;
print_r($o);
print_r((int) $o);
echo "\n";
var_dump($o);

?>
====DONE====
--EXPECTF--
====test1====
TestEmptyClass Object
(
)
string(60) "Object of class TestEmptyClass could not be converted to int"
int(1)
object(TestEmptyClass)#%d (0) {
}
====test2====
TestToIntClass Object
(
)
TestToIntClass::__toInt()
42
object(TestToIntClass)#%d (0) {
}
====DONE====
