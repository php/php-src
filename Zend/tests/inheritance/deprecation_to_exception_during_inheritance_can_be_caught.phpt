--TEST--
Deprecation promoted to exception during inheritance
--SKIPIF--
<?php
if (getenv('SKIP_PRELOAD')) die('skip Error handler not active during preloading');
?>
--FILE--
<?php

set_error_handler(function($code, $message) {
    throw new Exception($message);
});

try {
    class C extends DateTime {
        public function getTimezone() {}
        public function getTimestamp() {}
    };
} catch (Exception $e) {
    printf("%s: %s\n", $e::class, $e->getMessage());
}

var_dump(new C());

?>
--EXPECTF--
Exception: Return type of C::getTimezone() should either be compatible with DateTime::getTimezone(): DateTimeZone|false, or the #[\ReturnTypeWillChange] attribute should be used to temporarily suppress the notice
object(C)#%d (3) {
  ["date"]=>
  string(%d) "%s"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
