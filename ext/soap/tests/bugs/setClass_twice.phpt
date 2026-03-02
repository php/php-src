--TEST--
SOAP Server: SoapServer::setClass() twice
--EXTENSIONS--
soap
--FILE--
<?php
class Foo {
  function test() {
    return "Hello World";
  }
}

$server = new SoapServer(null,array('uri'=>"http://testuri.org"));
$server->setClass(Foo::class, new stdClass, []);
$server->setClass(Foo::class, new stdClass, []);

echo "Done\n";
?>
--EXPECT--
Done
