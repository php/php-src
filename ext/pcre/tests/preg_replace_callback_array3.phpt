--TEST--
preg_replace_callback_array() 3
--FILE--
<?php
$code = "test-EXECUTE_DATA-xcvxcv-ZEND_VM_DISPATCH_TO_HELPER";

$code = preg_replace_callback_array(
                array(
                    "/EXECUTE_DATA/m" => function($matches) { return "execute_data";},
                    "/ZEND_VM_DISPATCH_TO_HANDLER/m" => function($matches) { return "handler"; },
                    "/ZEND_VM_DISPATCH_TO_HELPER/m" => function($matches) { return "helper"; },
                    "/ZEND_VM_DISPATCH_TO_HELPER_EX/m" => function($matches) { return "helper_ex"; },
                ),
                $code);

var_dump($code);
$code = array("test-EXECUTE_DATA-ZEND_VM_DISPATCH_TO_HELPER_EX-test",
                "test-sdf-xcvxcv-ZEND_VM_DISPATCH_TO_HELPER_EX-test-EXECUTE_DATA-test");

$code = preg_replace_callback_array(
                array(
                    "/EXECUTE_DATA/m" => function($matches) { return "execute_data";},
                    "/ZEND_VM_DISPATCH_TO_HANDLER/m" => function($matches) { return "handler"; },
                    "/ZEND_VM_DISPATCH_TO_HELPER/m" => function($matches) { return "helper"; },
                ),
                $code, -1, $count);

var_dump($code, $count);

?>
--EXPECTF--	
string(31) "test-execute_data-xcvxcv-helper"
array(2) {
  [0]=>
  string(32) "test-execute_data-helper_EX-test"
  [1]=>
  string(48) "test-sdf-xcvxcv-helper_EX-test-execute_data-test"
}
int(4)
