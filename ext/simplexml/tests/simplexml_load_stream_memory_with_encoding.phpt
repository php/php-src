--TEST--
simplexml_load_stream() - from memory stream with encoding
--EXTENSIONS--
simplexml
--FILE--
<?php

$tmp = fopen("php://memory", "w+");
fwrite($tmp, '<?xml version="1.0" encoding="SHIFT_JIS"?><root>ééé</root>');
rewind($tmp);
$sxe1 = simplexml_load_stream($tmp, encoding: 'UTF-8');
rewind($tmp);
$sxe2 = simplexml_load_stream($tmp);
fclose($tmp);

var_dump($sxe1, $sxe2);

?>
--EXPECTF--
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(6) "ééé"
}
object(SimpleXMLElement)#%d (1) {
  [0]=>
  string(18) "ﾃｩﾃｩﾃｩ"
}
