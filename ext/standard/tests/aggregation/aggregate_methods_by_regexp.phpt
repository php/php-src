--TEST--
aggregating methods matching regular expression
--SKIPIF--
<?php if (!function_exists('aggregate_properties_by_regexp')) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
include dirname(__FILE__) . "/aggregate.lib";

$obj = new simple();
aggregate_methods_by_regexp($obj, 'helper', '/^do/');
print implode(',', get_class_methods($obj))."\n";
$obj2 = new simple();
aggregate_methods_by_regexp($obj2, 'helper', '/^do/', true);
print implode(',', get_class_methods($obj2))."\n";
?>
--EXPECT--
I'm alive!
simple,do_this,do_that
I'm alive!
simple,just_another_method
