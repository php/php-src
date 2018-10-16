--TEST--
ZE2 class constants and scope
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
Warning: Use of undefined constant FATAL - assumed 'FATAL' (this will throw an Error in a future version of PHP) in %sconstants_scope_001.php on line %d
FATAL = FATAL
self::FATAL = Fatal error
self::FATAL = Worst error
parent::FATAL = Fatal error
