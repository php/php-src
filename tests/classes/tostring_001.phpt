--TEST--
ZE2 __toString()
--FILE--
<?php

class test1
{
}

class test2
{
    function __toString()
    {
    	echo __METHOD__ . "()\n";
        return "Converted\n";
    }
}

class test3
{
    function __toString()
    {
    	echo __METHOD__ . "()\n";
        return 42;
    }
}
echo "====test1====\n";
$o = new test1;
print_r($o);
try {
    var_dump((string)$o);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump($o);

echo "====test2====\n";
$o = new test2;
print_r($o);
print $o;
var_dump($o);
echo "====test3====\n";
echo $o;

echo "====test4====\n";
echo "string:".$o;

echo "====test5====\n";
echo 1 . $o;
echo 1 , $o;

echo "====test6====\n";
echo $o . $o;
echo $o , $o;

echo "====test7====\n";
$ar = array();
$ar[$o->__toString()] = "ERROR";
echo $ar[$o];

echo "====test8====\n";
var_dump(trim($o));
var_dump(trim((string)$o));

echo "====test9====\n";
echo sprintf("%s", $o);

echo "====test10====\n";
$o = new test3;
var_dump($o);
try {
    echo $o;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
====DONE====
--EXPECTF--
====test1====
test1 Object
(
)
Object of class test1 could not be converted to string
object(test1)#1 (0) {
}
====test2====
test2 Object
(
)
test2::__toString()
Converted
object(test2)#3 (0) {
}
====test3====
test2::__toString()
Converted
====test4====
test2::__toString()
string:Converted
====test5====
test2::__toString()
1Converted
1test2::__toString()
Converted
====test6====
test2::__toString()
test2::__toString()
Converted
Converted
test2::__toString()
Converted
test2::__toString()
Converted
====test7====
test2::__toString()

Warning: Illegal offset type in %s on line %d
====test8====
test2::__toString()
string(9) "Converted"
test2::__toString()
string(9) "Converted"
====test9====
test2::__toString()
Converted
====test10====
object(test3)#1 (0) {
}
test3::__toString()
Method test3::__toString() must return a string value
====DONE====
