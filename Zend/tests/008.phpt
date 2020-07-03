--TEST--
define() tests
--FILE--
<?php

require __DIR__ . '/constants_helpers.inc';

tchelper_define(array(1,2,3,4,5), 1);
tchelper_define("TRUE", 1);
tchelper_define(" ", 1);
tchelper_define("[[[", 2);
tchelper_define("test const", 3);
tchelper_define("test const", 3);
tchelper_define("test", array(1));
tchelper_define("test1", fopen(__FILE__, 'r'));
tchelper_define("test2", new stdclass);

var_dump(constant(" "));
var_dump(constant("[[["));
var_dump(constant("test const"));

echo "Done\n";
?>
--EXPECT--
>> define(array, integer);
TypeError :: define(): Argument #1 ($constant_name) must be of type string, array given

>> define("TRUE", integer);
ValueError :: Constant TRUE already defined

>> define(" ", integer);
true

>> define("[[[", integer);
true

>> define("test const", integer);
true

>> define("test const", integer);
ValueError :: Constant test const already defined

>> define("test", array);
true

>> define("test1", resource);
true

>> define("test2", object);
TypeError :: Constants may only evaluate to scalar values, arrays or resources

int(1)
int(2)
int(3)
Done
