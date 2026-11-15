--TEST--
date_parse() doesn't reach hard limit
--INI--
memory_limit=64M
--FILE--
<?php
$result = date_parse(str_repeat("\x01", 4 * 1024 * 1024));
var_dump(count($result['errors']));
echo $result['errors'][-1], "\n";
?>
--EXPECT--
int(17)
There are more than 16 errors, which are not returned
