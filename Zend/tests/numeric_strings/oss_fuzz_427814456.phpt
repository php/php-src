--TEST--
OSS-Fuzz #427814456
--FILE--
<?php
set_error_handler(function(){unset($GLOBALS['x']);});
$x = str_repeat("3e33", random_int(2, 2));
$x & true;
echo "Done\n";
?>
--EXPECT--
Done
