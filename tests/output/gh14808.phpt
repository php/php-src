--TEST--
GH-14808 (Unexpected null pointer in Zend/zend_string.h with empty output buffer)
--EXTENSIONS--
iconv
--FILE--
<?php
var_dump($args);
ob_start('ob_iconv_handler');
ob_clean();
var_dump(ob_get_contents());
?>
--EXPECTF--
Warning: Undefined variable $args in %s on line %d
NULL
string(0) ""
