--TEST--
output buffering - ob_list_handlers
--FILE--
<?php
print_r(ob_list_handlers());

ob_start();
print_r(ob_list_handlers());

ob_start();
print_r(ob_list_handlers());

ob_end_flush();
print_r(ob_list_handlers());

ob_end_flush();
print_r(ob_list_handlers());
?>
--EXPECT--
Array
(
)
Array
(
    [0] => default output handler
)
Array
(
    [0] => default output handler
    [1] => default output handler
)
Array
(
    [0] => default output handler
)
Array
(
)
