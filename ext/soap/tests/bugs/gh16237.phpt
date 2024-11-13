--TEST--
GH-16237 (Segmentation fault when cloning SoapServer)
--EXTENSIONS--
soap
--FILE--
<?php

$server = new SoapServer(null, ['uri'=>"http://testuri.org"]);
try {
    clone $server;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Trying to clone an uncloneable object of class SoapServer
