--TEST--
ZE2 __toString()
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class test1 {
}

class test2 {
    function __toString() {
    	echo __METHOD__ . "()\n";
        return "Converted\n";
    }
}

echo "====test1====\n";
$o = new test1;
print_r($o);
var_dump((string)$o);
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
echo 1 . $o;

echo "====test6====\n";
echo $o.$o;
echo $o,$o;

echo "====test7====\n";
$ar = array();
$ar[$o->__toString()] = "ERROR";
echo $ar[$o];

echo "====test8====\n";
var_dump(trim($o));
var_dump(trim((string)$o));

echo "====test9====\n";
echo sprintf("%s", $o);
?>
====DONE!====
--EXPECTF--
====test1====
test1 Object
(
)
string(12) "Object id #%d"
object(test1)#%d (0) {
}
====test2====
test2 Object
(
)
test2::__toString()
Converted
object(test2)#%d (%d) {
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
test2::__toString()
1Converted
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

Warning: Illegal offset type in %stostring.php on line %d
====test8====
test2::__toString()
string(9) "Converted"
test2::__toString()
string(9) "Converted"
====test9====
test2::__toString()
Converted
====DONE!====
--UEXPECTF--
====test1====
test1 Object
(
)
string(12) "Object id #%d"
object(test1)#%d (0) {
}
====test2====
test2 Object
(
)
test2::__toString()
Converted
object(test2)#%d (%d) {
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
test2::__toString()
1Converted
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

Warning: Illegal offset type in %stostring.php on line %d
====test8====
test2::__toString()
unicode(9) "Converted"
test2::__toString()
unicode(9) "Converted"
====test9====
test2::__toString()
Converted
====DONE!====
