--TEST--
ReflectionFunction basic tests
--INI--
opcache.save_comments=1
opcache.load_comments=1
--FILE--
<?php

/**
hoho
*/
function test ($a, $b = 1, $c = "") {
	static $var = 1;
}

$func = new ReflectionFunction("test");

var_dump($func->export("test"));
echo "--getName--\n";
var_dump($func->getName());
echo "--isInternal--\n";
var_dump($func->isInternal());
echo "--isUserDefined--\n";
var_dump($func->isUserDefined());
echo "--getFilename--\n";
var_dump($func->getFilename());
echo "--getStartline--\n";
var_dump($func->getStartline());
echo "--getEndline--\n";
var_dump($func->getEndline());
echo "--getDocComment--\n";
var_dump($func->getDocComment());
echo "--getStaticVariables--\n";
var_dump($func->getStaticVariables());
echo "--invoke--\n";
var_dump($func->invoke(array(1,2,3)));
echo "--invokeArgs--\n";
var_dump($func->invokeArgs(array(1,2,3)));
echo "--returnsReference--\n";
var_dump($func->returnsReference());
echo "--getParameters--\n";
var_dump($func->getParameters());
echo "--getNumberOfParameters--\n";
var_dump($func->getNumberOfParameters());
echo "--getNumberOfRequiredParameters--\n";
var_dump($func->getNumberOfRequiredParameters());

echo "Done\n";

?>
--EXPECTF--	
/**
hoho
*/
Function [ <user> function test ] {
  @@ %s009.php 6 - 8

  - Parameters [3] {
    Parameter #0 [ <required> $a ]
    Parameter #1 [ <optional> $b = 1 ]
    Parameter #2 [ <optional> $c = '' ]
  }
}

NULL
--getName--
string(4) "test"
--isInternal--
bool(false)
--isUserDefined--
bool(true)
--getFilename--
string(%d) "%s009.php"
--getStartline--
int(6)
--getEndline--
int(8)
--getDocComment--
string(11) "/**
hoho
*/"
--getStaticVariables--
array(1) {
  ["var"]=>
  int(1)
}
--invoke--
NULL
--invokeArgs--
NULL
--returnsReference--
bool(false)
--getParameters--
array(3) {
  [0]=>
  &object(ReflectionParameter)#2 (1) {
    ["name"]=>
    string(1) "a"
  }
  [1]=>
  &object(ReflectionParameter)#3 (1) {
    ["name"]=>
    string(1) "b"
  }
  [2]=>
  &object(ReflectionParameter)#4 (1) {
    ["name"]=>
    string(1) "c"
  }
}
--getNumberOfParameters--
int(3)
--getNumberOfRequiredParameters--
int(1)
Done
