--TEST--
Bug #25756 (validate_schema_file() broken)
--FILE--
<?php
$dir = dirname(__FILE__);
$valid_schema_file = "$dir/bug25756.xsd";
$invalid_schema_file = "$dir/bug25756_1.xml";
$xml_file_1 = "$dir/bug25756_1.xml";
$xml_file_2 = "$dir/bug25756_2.xml";

$s = simplexml_load_file($xml_file_1);
var_dump($s);
var_dump($s->validate_schema_file($valid_schema_file));
var_dump($s->validate_schema_file($invalid_schema_file));
$s = simplexml_load_file($xml_file_2);
var_dump($s);
var_dump($s->validate_schema_file($valid_schema_file));
?>
--EXPECTF--
object(simplexml_element)#1 (1) {
  ["items"]=>
  object(simplexml_element)#2 (1) {
    ["item"]=>
    array(2) {
      [0]=>
      object(simplexml_element)#3 (2) {
        ["product-name"]=>
        string(3) "abc"
        ["quantity"]=>
        string(3) "123"
      }
      [1]=>
      object(simplexml_element)#4 (2) {
        ["product-name"]=>
        string(3) "def"
        ["quantity"]=>
        string(3) "456"
      }
    }
  }
}
bool(true)

Warning: Unknown: Malformed XML Schema in %s on line %d
bool(false)
object(simplexml_element)#5 (1) {
  ["items"]=>
  object(simplexml_element)#1 (1) {
    ["item"]=>
    array(2) {
      [0]=>
      object(simplexml_element)#6 (2) {
        ["product-name"]=>
        string(3) "abc"
        ["quantity"]=>
        string(3) "abc"
      }
      [1]=>
      object(simplexml_element)#7 (2) {
        ["product-name"]=>
        string(3) "abc"
        ["quantity"]=>
        string(3) "123"
      }
    }
  }
}
bool(false)
