--TEST--
Bug #65391 (Unable to send vary header user-agent when ob_start('ob_gzhandler') is called)
--SKIPIF--
<?php
extension_loaded("zlib") or die("skip need zlib");
?>
--GET--
dummy=1
--INI--
expose_php=On
--FILE--
<?php
header("Vary: Cookie");
ob_start("ob_gzhandler");

// run-tests cannot test for a multiple Vary header
ob_flush();
print_r(headers_list());

?>
Done
--EXPECTF--
Array
(
    [0] => X-Powered-By: PHP/%s
    [1] => Vary: Cookie
    [2] => Vary: Accept-Encoding
)
Done
