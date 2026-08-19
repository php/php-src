--TEST--
SOAP Server 8: setclass and getfunctions
--EXTENSIONS--
soap
--FILE--
<?php
class Foo {

  function __construct() {
  }

  function test() {
    return $this->str;
  }
}

$server = new SoapServer(null,array('uri'=>"http://testuri.org"));
$server->setClass("Foo");
var_dump($server->getFunctions());
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  string(11) "__construct"
  [1]=>
  string(4) "test"
}
ok
