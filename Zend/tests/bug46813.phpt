--TEST--
Bug #46813: class_exists doesn`t work with fully qualified namespace
--FILE--
<?php
namespace test;
{
	class inner
	{
		
	}
}

$inner = new \test\inner();

echo "autoload == true:\n";
var_dump(class_exists('\test\inner', true));
var_dump(class_exists(b'\test\inner', true));
echo "autoload == false:\n";
var_dump(class_exists('\test\inner', true));
var_dump(class_exists(b'\test\inner', true));
?>
--EXPECT--
autoload == true:
bool(true)
bool(true)
autoload == false:
bool(true)
bool(true)
