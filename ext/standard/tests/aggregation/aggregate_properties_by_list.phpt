--TEST--
aggregating default properties specified in the list
--POST--
--GET--
--FILE--
<?php
include dirname(__FILE__) . "/aggregate.lib";

$obj = new simple();
aggregate_properties_by_list($obj, 'helper', array('my_prop', 'our_prop'));
print implode(',', array_keys(get_object_vars($obj)))."\n";
$obj2 = new simple();
aggregate_properties_by_list($obj2, 'helper', array('my_prop'), true);
print implode(',', array_keys(get_object_vars($obj2)))."\n";
?>
--EXPECT--
I'm alive!
simple_prop,my_prop,our_prop
I'm alive!
simple_prop,your_prop,our_prop
