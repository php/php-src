--TEST--
stream_filter_register() with a class name exist but does not extend php_user_filter and defines a $filtername prop with different type
--FILE--
<?php
class foo {
	public array $filtername;
}

var_dump(stream_filter_register("invalid_filter", "foo"));

var_dump(stream_filter_append(STDOUT, "invalid_filter"));

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
bool(true)

Deprecated: Creation of dynamic property foo::$params is deprecated in %s on line %d

Fatal error: Uncaught TypeError: Cannot assign string to property foo::$filtername of type array in %s:%d
Stack trace:
#0 %s(%d): stream_filter_append(Resource id #2, 'invalid_filter')
#1 {main}
  thrown in %s on line %d

Fatal error: Invalid callback foo::filter, class foo does not have a method "filter" in Unknown on line 0
