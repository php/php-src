--TEST--
Comparing uninitialized SimpleXMLElement instances must not segfault
--EXTENSIONS--
simplexml
--FILE--
<?php
class MySXE extends SimpleXMLElement {
    public function __construct() {}
}
$a = new MySXE;
$b = new MySXE;
echo "self: ";
var_dump($a == $a);
echo "equal: ";
var_dump($a == $b);
echo "identical: ";
var_dump($a === $b);
$c = simplexml_load_string('<r/>');
echo "uninit vs init: ";
var_dump($a == $c);
echo "done\n";
?>
--EXPECT--
self: bool(true)
equal: bool(false)
identical: bool(false)
uninit vs init: bool(false)
done
