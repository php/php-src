--TEST--
Bug #67572 - SimpleXMLElement not parsing \n correctly
--SKIPIF--
<?php
if (!extension_loaded("simplexml")) die("skip SimpleXML not available");
?>
--FILE--
<?php
$foo = 'bar';
print "regular string ... ";
var_dump(empty($foo));

$xml = simplexml_load_string("<xml><something>somevalue</something></xml>");
$xml2 = simplexml_load_string("<xml>\n<something>somevalue</something>\n</xml>");

foreach($xml as $key => $value) {
    print "$key = $value ... ";
    var_dump(empty($value));
    var_dump($value == false);
}

foreach($xml2 as $key => $value) {
    print "$key = $value ... ";
    var_dump(empty($value));
    var_dump($value == false);
}
?>
--EXPECT--
regular string ... bool(false)
something = somevalue ... bool(false)
bool(false)
something = somevalue ... bool(false)
bool(false)
