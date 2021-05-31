--TEST--
Deprecation promoted to exception should result in fatal error during inheritance
--FILE--
<?php

set_error_handler(function($code, $message) {
    throw new Exception($message);
});

$class = new class extends DateTime {
    public function getTimezone() {}
};

?>
--EXPECTF--
Fatal error: During inheritance of DateTime: Uncaught Exception: Declaration of DateTime@anonymous::getTimezone() should be compatible with DateTime::getTimezone(): DateTimeZone|false in %s:%d
Stack trace:
#0 %s(%d): {closure}(8192, 'Declaration of ...', '%s', 8)
#1 {main} in %s on line %d
