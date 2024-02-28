--TEST--
Deprecation promoted to exception should result in fatal error during inheritance
--SKIPIF--
<?php
if (getenv('SKIP_PRELOAD')) die('skip Error handler not active during preloading');
?>
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
Fatal error: During inheritance of DateTime: Uncaught Exception: Return type of DateTime@anonymous::getTimezone() should either be compatible with DateTime::getTimezone(): DateTimeZone|false, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice in %s:%d
Stack trace:
#0 %s(%d): {closure:%s:%d}(8192, 'Return type of ...', '%s', 8)
#1 {main} in %s on line %d
