--TEST--
Test HTTP stream URI parsing - error - invalid URI
--INI--
allow_url_fopen=1
--FILE--
<?php

// hits ext/uri parsing and fails
// before connection is attempted
var_dump(@fopen("http://", "r"));

?>
--EXPECT--
bool(false)
