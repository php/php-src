--TEST--
SOAP Server 7: addfunction and getfunctions
--EXTENSIONS--
soap
--FILE--
<?php
function Add($x,$y) {
  return $x+$y;
}
function Sub($x,$y) {
  return $x-$y;
}

$server = new SoapServer(null,array('uri'=>"http://testuri.org"));
$server->addFunction(array("Sub","Add"));
var_dump($server->getFunctions());
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  string(3) "Sub"
  [1]=>
  string(3) "Add"
}
ok
