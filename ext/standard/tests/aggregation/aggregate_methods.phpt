--TEST--
aggregating all methods
--POST--
--GET--
--FILE--
<?php
include dirname(__FILE__) . "/aggregate.lib";

$obj = new simple();
aggregate_methods($obj, 'mixin');
$obj->mix_it();
print $obj->simple_prop."\n";
print implode(',', get_class_methods($obj))."\n";
print implode(',', array_keys(get_object_vars($obj)))."\n";
aggregate_methods($obj, 'moby');
$obj->mix_it();

?>
--EXPECT--
I'm alive!
mixing
100
simple,mix_it
simple_prop
mixing
