--TEST--
Phar: bug #71354: Heap corruption in tar/zip/phar parser.
--SKIPIF--
<?php if (!extension_loaded("phar")) die("skip"); ?>
--FILE--
<?php
$p = new PharData(__DIR__."/bug71354.tar");
var_dump($p['aaaa']->getContent());
?>
DONE
--EXPECT--
string(0) ""
DONE