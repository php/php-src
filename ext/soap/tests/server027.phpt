--TEST--
SOAP Server 27: setObject and getFunctions
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
class Foo {

  function Foo() {
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
  string(3) "Foo"
  [1]=>
  string(4) "test"
}
ok
