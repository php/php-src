--TEST--
SOAP Server 27: setObject and getFunctions
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

$foo = new Foo();
$server = new SoapServer(null,array('uri'=>"http://testuri.org"));
$server->setObject($foo);
var_dump($server->getfunctions());
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
