--TEST--
Serialize and namespaces
--FILE--
<?php
function autoload($fqcn) {
	list($namespace, $class)= explode('::', $fqcn);
	if ('power' == $namespace) {
		eval('namespace '.$namespace.' { class '.$class.' {}}');
	}
}

namespace Foo:Baz { 
	class Bar { } 
} 

class Baz { } 

echo "#### 1 ####\n";
$bar= new Foo:Baz::Bar(); 
var_dump($bar, serialize($bar), unserialize(serialize($bar)));

echo "#### 2 ####\n";
$baz= new Baz();
var_dump(serialize($baz), unserialize(serialize($baz)));

echo "#### 3 ####\n";
var_dump(unserialize('O:7:"Binford":0:{}'));

echo "#### 4 ####\n";
ini_set('unserialize_callback_func', 'autoload');
var_dump(unserialize('O:14:"Power::Binford":0:{}'));
echo "#### 5 ####\n";
var_dump(unserialize('O:14:"Idiot::Binford":0:{}'));
?>
--EXPECTF--
#### 1 ####
object(foo:baz::bar)#1 (0) {
}
string(14) "O:3:"bar":0:{}"
object(__PHP_Incomplete_Class)#2 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(3) "bar"
}
#### 2 ####
string(14) "O:3:"baz":0:{}"
object(baz)#3 (0) {
}
#### 3 ####
object(__PHP_Incomplete_Class)#3 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(7) "binford"
}
#### 4 ####
object(power::binford)#3 (0) {
}
#### 5 ####

Warning: unserialize(): Function autoload() hasn't defined the class it was called for in %s on line %d
object(__PHP_Incomplete_Class)#3 (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(14) "idiot::binford"
}
