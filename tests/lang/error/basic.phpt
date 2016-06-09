--TEST--
Uncatched exceptions are properly stored.
--FILE--
<?php

register_shutdown_function(function () {
	var_dump(error_get_last());
});

new DateTime('1/1/11111');

?>
--EXPECTF--
Fatal error: Uncaught Exception: DateTime::__construct(): Failed to parse time string (1/1/11111) at position 8 (1): Unexpected character in %s
Stack trace:
#0 %s: DateTime->__construct('1/1/11111')
#1 {main}
  thrown in %s
array(4) {
  ["type"]=>
  int(1)
  ["message"]=>
  string(327) "Uncaught Exception: DateTime::__construct(): Failed to parse time string (1/1/11111) at position 8 (1): Unexpected character in %s
Stack trace:
#0 %s: DateTime->__construct('1/1/11111')
#1 {main}
  thrown"
  ["file"]=>
  string(61) "%s"
  ["line"]=>
  int(%d)
}
