--TEST--
aggregating methods specified in the list
--POST--
--GET--
--FILE--
<?php
include dirname(__FILE__) . "/aggregate.lib";

$obj = new simple();
aggregate_methods_by_list($obj, 'helper', array('just_another_method'));
print implode(',', get_class_methods($obj))."\n";
$obj2 = new simple();
aggregate_methods_by_list($obj2, 'helper', array('just_another_method'), true);
print implode(',', get_class_methods($obj2))."\n";
$obj->just_another_method();
?>
--EXPECT--
I'm alive!
simple,just_another_method
I'm alive!
simple,do_this,do_that
yep, that's me
