--TEST--
Evaluation order during assignments.
--FILE--
<?php

function f() {
	echo "in f()\n";
	return "name";
}

function g() {
	echo "in g()\n";
	return "assigned value";
}


echo "\n\nOrder with local assignment:\n"; 
${f()} = g();
var_dump($name);

echo "\n\nOrder with array assignment:\n";
$a[f()] = g();
var_dump($a);

echo "\n\nOrder with object property assignment:\n";
$oa = new stdClass;
$oa->${f()} = g();
var_dump($oa);

echo "\n\nOrder with nested object property assignment:\n";
$ob = new stdClass;
$ob->o1 = new stdClass;
$ob->o1->o2 = new stdClass;
$ob->o1->o2->${f()} = g();
var_dump($ob);

echo "\n\nOrder with dim_list property assignment:\n";
$oc = new stdClass;
$oc->a[${f()}] = g();
var_dump($oc);


class C {
	public static $name = "original";
	public static $a = array(); 
	public static $string = "hello";
}
echo "\n\nOrder with static property assignment:\n";
C::${f()} = g();
var_dump(C::$name);

echo "\n\nOrder with static array property assignment:\n";
C::$a[f()] = g();
var_dump(C::$a);

echo "\n\nOrder with indexed string assignment:\n";
$string = "hello";
function getOffset() { 
	echo "in getOffset()\n";
	return 0;
}
function newChar() {
	echo "in newChar()\n";
	return 'j';
}
$string[getOffset()] = newChar();
var_dump($string);

echo "\n\nOrder with static string property assignment:\n";
C::$string[getOffset()] = newChar();
var_dump(C::$string);

?>
--EXPECTF--


Order with local assignment:
in f()
in g()
%string|unicode%(14) "assigned value"


Order with array assignment:
in f()
in g()
array(1) {
  [%u|b%"name"]=>
  %string|unicode%(14) "assigned value"
}


Order with object property assignment:
in f()
in g()
object(stdClass)#%d (1) {
  [%u|b%"assigned value"]=>
  %string|unicode%(14) "assigned value"
}


Order with nested object property assignment:
in f()
in g()
object(stdClass)#%d (1) {
  [%u|b%"o1"]=>
  object(stdClass)#%d (1) {
    [%u|b%"o2"]=>
    object(stdClass)#%d (1) {
      [%u|b%"assigned value"]=>
      %string|unicode%(14) "assigned value"
    }
  }
}


Order with dim_list property assignment:
in f()
in g()
object(stdClass)#%d (1) {
  [%u|b%"a"]=>
  array(1) {
    [%u|b%"assigned value"]=>
    %string|unicode%(14) "assigned value"
  }
}


Order with static property assignment:
in f()
in g()
%string|unicode%(14) "assigned value"


Order with static array property assignment:
in f()
in g()
array(1) {
  [%u|b%"name"]=>
  %string|unicode%(14) "assigned value"
}


Order with indexed string assignment:
in getOffset()
in newChar()
%string|unicode%(5) "jello"


Order with static string property assignment:
in getOffset()
in newChar()
%string|unicode%(5) "jello"
