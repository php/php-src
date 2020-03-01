--TEST--
Bug #79178	[parse_url] underscore at end of host
--FILE--
<?php

// Should return false because HOST contains a control character (PHP_EOL)
var_dump(parse_url('https://php.net' . PHP_EOL, PHP_URL_HOST));

?>
--EXPECT--
bool(false)
