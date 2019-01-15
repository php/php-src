--TEST--
new DOMProcessingInstruction("php-class") should fail
--CREDITS--
Adam Martinson
--SKIPIF--
<?php
require_once('skipif.inc');
if (!method_exists('DOMDocument', 'createObject'))
	die('DOM_PHP_CLASS_PI_DISABLED');
?>
--FILE--
<?php
try {
	$Pi = new DOMProcessingInstruction("php-class");
} catch (DOMException $Ex) {
	echo $Ex->getMessage() . "\n";
	if ($Ex->getCode() == DOM_PHP_ERR) {
		echo "OK\n";
	}
}
?>
--EXPECT--
<?php-class ...?> processing instruction may not be explicitly created
OK
