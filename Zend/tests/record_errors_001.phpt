--TEST--
Error recording in error handler
--SKIPIF--
<?php
if (getenv('SKIP_PRELOAD')) die('skip Error handler not active during preloading');
?>
--FILE--
<?php
set_error_handler(function($code, $msg) {
	echo "Error: $msg\n";
    new class extends DateTime {
    };
});
new class extends DateTime {
    function getTimezone() {}
};
?>
--EXPECT--
Error: Return type of class@anonymous::getTimezone() should either be compatible with DateTime::getTimezone(): DateTimeZone|false, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice
