--TEST--
stream_filter_register() with a class that has a onclose method that throws
--FILE--
<?php
class foo extends php_user_filter {
	public function onclose(): void {
		throw new Exception("No");
	}
}

var_dump(stream_filter_register("invalid_filter", "foo"));

var_dump(stream_filter_append(STDOUT, "invalid_filter"));

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
bool(true)
resource(4) of type (stream filter)

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d
bool(false)

Fatal error: Uncaught Exception: No in %s:%d
Stack trace:
#0 [internal function]: foo->onclose()
#1 {main}
  thrown in %s on line %d
