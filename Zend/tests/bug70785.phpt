--TEST--
Bug #70785 (Infinite loop due to exception during identical comparison)
--FILE--
<?php

set_error_handler(function($no, $msg) {
	throw new Exception($msg);
});

try {
	if ($a === null) { // ZEND_VM_SMART_BRANCH
		undefined_function('Null');
	}
} catch (Exception $e) {
}

try  {
	$c === 3; // ZEND_VM_NEXT_OPCODE
	undefined_function();
} catch (Exception $e) {
}
?>
okey
--EXPECT--
okey
