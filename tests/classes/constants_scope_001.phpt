--TEST--
ZE2 class constants and scope
--SKIPIF--
<?php if (version_compare(zend_version(), '2.0.0-dev', '<')) die('skip ZendEngine 2 needed'); ?>
--FILE--
<?php

class ErrorCodes {
	const FATAL = "Fatal error\n";
	const WARNING = "Warning\n";
	const INFO = "Informational message\n";

	static function print_fatal_error_codes() {
		echo "FATAL = " . FATAL . "\n";
		echo "self::FATAL = " . self::FATAL;
    }
}

class ErrorCodesDerived extends ErrorCodes {
	const FATAL = "Worst error\n";
	static function print_fatal_error_codes() {
		echo "self::FATAL = " . self::FATAL;
		echo "parent::FATAL = " . parent::FATAL;
    }
}

/* Call the static function and move into the ErrorCodes scope */
ErrorCodes::print_fatal_error_codes();
ErrorCodesDerived::print_fatal_error_codes();

?>
--EXPECTF--

Notice: Use of undefined constant FATAL - assumed 'FATAL' in %sconstants_scope_001.php on line %d
FATAL = FATAL
self::FATAL = Fatal error
self::FATAL = Worst error
parent::FATAL = Fatal error
