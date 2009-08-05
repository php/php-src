--TEST--
Test is_resource() function : basic functionality 
--FILE--
<?php
/* Prototype  : bool is_resource  ( mixed $var  )
 * Description:  Finds whether a variable is a resource
 * Source code: ext/standard/type.c
 */		

echo "*** Testing is_resource() : basic functionality ***\n";

class Hello {
  public function SayHello($arg) {
  	echo "Hello\n";
  } 
}


$vars = array(
	false,
	true,
	10,
	10.5,
	"Helo World",
	array(1,2,3,4,5),
	NULL,
	new Hello());
	
$types = array(
	"bool=false",
	"bool=true",
	"integer",
	"double",
	"string",
	"array",
	"NULL",
	"object");	

echo "\nNon-resource type cases\n";

for ($i=0; $i < count($vars); $i++) {
	if (is_resource($vars[$i])) {
		echo $types[$i]. " test returns TRUE\n";
	} else {
		echo $types[$i]. " test returns FALSE\n";
	}
}	

$res = fopen(__FILE__, "r");
echo "\nResource type..var_dump after file open returns\n";
var_dump($res);
echo "Resource type..after file open  is_resource() returns";
if (is_resource($res)) {
	echo " TRUE\n";
} else {
	echo " FALSE\n";
}

fclose($res);
echo "\nResource type..var_dump after file close returns\n";
var_dump($res);
echo "Resource type..after file close is_resource() returns";
if (is_resource($res)) {
	echo " TRUE\n";
} else {
	echo " FALSE\n";
}	


?>
===DONE===
--EXPECTF--
*** Testing is_resource() : basic functionality ***

Non-resource type cases
bool=false test returns FALSE
bool=true test returns FALSE
integer test returns FALSE
double test returns FALSE
string test returns FALSE
array test returns FALSE
NULL test returns FALSE
object test returns FALSE

Resource type..var_dump after file open returns
resource(%d) of type (%s)
Resource type..after file open  is_resource() returns TRUE

Resource type..var_dump after file close returns
resource(%d) of type (Unknown)
Resource type..after file close is_resource() returns FALSE
===DONE===
