--TEST--
stream_filter_register() with a class name exist but does not extend php_user_filter and cannot have dynamic properties
--FILE--
<?php

var_dump(stream_filter_register("invalid_filter", "SensitiveParameter"));

var_dump(stream_filter_append(STDOUT, "invalid_filter"));

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
bool(true)

Fatal error: Uncaught Error: Cannot create dynamic property SensitiveParameter::$filtername in %s:%d
Stack trace:
#0 %s(%d): stream_filter_append(Resource id #%d, 'invalid_filter')
#1 {main}

Next Error: Cannot create dynamic property SensitiveParameter::$params in %s:%d
Stack trace:
#0 %s(%d): stream_filter_append(Resource id #%d, 'invalid_filter')
#1 {main}
  thrown in %s on line %d

Fatal error: Invalid callback SensitiveParameter::filter, class SensitiveParameter does not have a method "filter" in Unknown on line 0
