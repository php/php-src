--TEST--
Bug #25756 (validate_schema_file() broken)
--FILE--
<?php
$dir = dirname(__FILE__);
$valid_schema_file = "$dir/bug25756.xsd";
$invalid_schema_file = "$dir/bug25756_1.xml";
$xml_file_valid = "$dir/bug25756_1.xml";
$xml_file_invalid = "$dir/bug25756_2.xml";

$s = simplexml_load_file($xml_file_valid);
var_dump($s);
var_dump($s->validate_schema_file($valid_schema_file));
var_dump($s->validate_schema_file($invalid_schema_file));
$s = simplexml_load_file($xml_file_invalid);
var_dump($s);
var_dump($s->validate_schema_file($valid_schema_file));
?>
--EXPECTF--
object(simplexml_element)#%d (1) {
  ["items"]=>
  object(simplexml_element)#%d (1) {
    ["item"]=>
    array(2) {
      [0]=>
      object(simplexml_element)#%d (2) {
        ["product-name"]=>
        string(3) "abc"
        ["quantity"]=>
        string(3) "123"
      }
      [1]=>
      object(simplexml_element)#%d (2) {
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
object(simplexml_element)#%d (1) {
  ["items"]=>
  object(simplexml_element)#%d (1) {
    ["item"]=>
    array(2) {
      [0]=>
      object(simplexml_element)#%d (2) {
        ["product-name"]=>
        string(3) "abc"
        ["quantity"]=>
        string(3) "abc"
      }
      [1]=>
      object(simplexml_element)#%d (2) {
        ["product-name"]=>
        string(3) "abc"
        ["quantity"]=>
        string(3) "123"
      }
    }
  }
}

Warning: Element quantity: failed to validate basic type decimal in %s on line %d
bool(false)
