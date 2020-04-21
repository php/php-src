--TEST--
Bug #79065 (DOM classes do not expose properties to Reflection)
--SKIPIF--
<?php
if (!extension_loaded('dom')) die('skip dom extension not available');
?>
--FILE--
<?php
$dom = new DOMDocument;
$dom->loadHTML('<b>test</b>');
var_dump(count(get_object_vars($dom)));

$ro = new ReflectionObject($dom);
var_dump(count($ro->getProperties()));
var_dump($ro->hasProperty("textContent"));
$rp = $ro->getProperty("textContent");
var_dump($rp);
var_dump($rp->getValue($dom));
?>
--EXPECTF--
int(35)
int(35)
bool(true)
object(ReflectionProperty)#%d (2) {
  ["name"]=>
  string(11) "textContent"
  ["class"]=>
  string(11) "DOMDocument"
}
string(4) "test"
