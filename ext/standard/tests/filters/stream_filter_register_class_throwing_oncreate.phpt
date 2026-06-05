--TEST--
stream_filter_register() with a class that has a onCreate method that throws
--FILE--
<?php
class foo extends php_user_filter {
	public function onCreate(): bool {
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

Fatal error: Uncaught Exception: No in %s:%d
Stack trace:
#0 [internal function]: foo->onCreate()
#1 %s(%d): stream_filter_append(Resource id #2, 'invalid_filter')
#2 {main}
  thrown in %s on line %d
