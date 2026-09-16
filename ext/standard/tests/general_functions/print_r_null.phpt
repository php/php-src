--TEST--
Test print_r() function with null
--FILE--
<?php

echo "null:\n";
print_r(null, false);
// $ret_string captures the output
$ret_string = print_r(null, true);
echo "\n$ret_string\n";

?>
DONE
--EXPECT--
null:


DONE
