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
        return "Converted";
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
var_dump((string)$o);
var_dump($o);
echo "====done!====\n";
?>
--EXPECTF--
====test1====
test1 Object
(
)

Notice: Object of class test1 could not be converted to string in %stostring.php on line %d
string(6) "Object"
object(test1)#%d (0) {
}
====test2====
test2 Object
(
)
string(9) "Converted"
object(test2)#%d (0) {
}
====done!====
