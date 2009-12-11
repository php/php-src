--TEST--
ob_get_status() function basic test
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php
ob_start();
$status = ob_get_status(true);
ob_end_clean();
var_dump($status);
?>
--EXPECT--
array(1) {
  [0]=>
  array(7) {
    [u"name"]=>
    unicode(22) "default output handler"
    [u"type"]=>
    int(0)
    [u"flags"]=>
    int(112)
    [u"level"]=>
    int(0)
    [u"chunk_size"]=>
    int(0)
    [u"buffer_size"]=>
    int(16384)
    [u"buffer_used"]=>
    int(0)
  }
}
