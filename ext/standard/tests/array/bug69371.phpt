--TEST--
Bug #69371 (Hash table collision leads to inaccessible array keys)
--FILE--
<?php
$array = array(
    "d6_node_type" => 1,
    "d6_filter_format" => 2,
    "d6_user" => 3,
    "d6_field_instance_widget_settings" => 4,
    "d6_field_formatter_settings" => 5,
);

$weights = array(
    5, 1, 3, 2, 0
);
array_multisort($weights, SORT_DESC, SORT_NUMERIC, $array);

var_dump($array["d6_node_type"]);

?>
--EXPECT--
int(1)
