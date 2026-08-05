--TEST--
Negative SimpleXML element offsets must not alias the first element
--EXTENSIONS--
simplexml
--FILE--
<?php
$xml = simplexml_load_string('<r><item>a</item><item>b</item><item>c</item></r>');
$items = $xml->item;

echo "isset[-1]: ";
var_dump(isset($items[-1]));
echo "read[-1]: ";
var_dump($items[-1]);
echo "read[0]: ";
var_dump((string)$items[0]);

$items[-1] = 'Z';
echo "after negative write: ", $xml->asXML();

$items[5] = 'P';
echo "after out-of-range write: ", $xml->asXML();
?>
--EXPECTF--
isset[-1]: bool(false)
read[-1]: NULL
read[0]: string(1) "a"

Warning: main(): Cannot add element item number -1 when only 3 such elements exist in %s on line %d
after negative write: <?xml version="1.0"?>
<r><item>a</item><item>b</item><item>c</item></r>

Warning: main(): Cannot add element item number 5 when only 3 such elements exist in %s on line %d
after out-of-range write: <?xml version="1.0"?>
<r><item>a</item><item>b</item><item>c</item><item>P</item></r>
