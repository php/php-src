--TEST--
stream_filter_register() with a class name that does not exist
--FILE--
<?php

var_dump(stream_filter_register("not_existing_filter", "not_existing"));

stream_filter_append(STDOUT, "not_existing_filter");

$out = fwrite(STDOUT, "Hello\n");
var_dump($out);

?>
--EXPECTF--
bool(true)

Warning: stream_filter_append(): User-filter "not_existing_filter" requires class "not_existing", but that class is not defined in %s on line %d

Warning: stream_filter_append(): Unable to create or locate filter "not_existing_filter" in %s on line %d
Hello
int(6)
