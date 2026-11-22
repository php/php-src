--TEST--
Request globals are reset between requests
--FILE--
<?php

var_dump(zend_test_set_request_value(1));

?>
--EXPECT--
int(0)
