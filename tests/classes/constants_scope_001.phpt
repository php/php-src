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
		echo "FATAL = " . FATAL;
		echo "self::FATAL = " . self::FATAL;
    }
}

/* Call the static function and move into the ErrorCodes scope */
ErrorCodes::print_fatal_error_codes();

?>
--EXPECT--
FATAL = Fatal error
self::FATAL = Fatal error
