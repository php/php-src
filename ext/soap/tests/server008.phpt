--TEST--
SOAP Server 8: setclass and getfunctions
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

$server = new soapserver(null,array('uri'=>"http://testuri.org"));
$server->setclass("Foo");
var_dump($server->getfunctions());
echo "ok\n";
?>
--EXPECT--
array(2) {
  [0]=>
  unicode(3) "Foo"
  [1]=>
  unicode(4) "test"
}
ok
