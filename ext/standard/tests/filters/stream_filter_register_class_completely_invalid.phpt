--TEST--
stream_filter_register() with a class name that exist but does not extend php_user_filter nor mock anything
--FILE--
<?php
class foo {

}

var_dump(stream_filter_register("invalid_filter", "foo"));

var_dump(stream_filter_append(STDOUT, "invalid_filter"));

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
bool(true)

Deprecated: Creation of dynamic property foo::$filtername is deprecated in %s on line %d

Deprecated: Creation of dynamic property foo::$params is deprecated in %s on line %d
resource(%d) of type (stream filter)

Warning: fwrite(): Unprocessed filter buckets remaining on input brigade in %s on line %d

Fatal error: Uncaught Error: Invalid callback foo::filter, class foo does not have a method "filter" in %s:%d
Stack trace:
#0 %s(%d): fwrite(Resource id #%d, 'Hello\n')
#1 {main}
  thrown in %s on line %d

Fatal error: Invalid callback foo::filter, class foo does not have a method "filter" in Unknown on line 0
