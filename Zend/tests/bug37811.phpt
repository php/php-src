--TEST--
Bug #37811 (define not using toString on objects)
--FILE--
<?php

require __DIR__ . '/constants_helpers.inc';

class TestClass
{
    function __toString()
    {
        return "Foo";
    }
}

tchelper_define("Bar",new TestClass);

tchelper_define("Baz",new stdClass);

?>
===DONE===
--EXPECT--
>> define("Bar", object);
true

>> define("Baz", object);
TypeError :: Constants may only evaluate to scalar values, arrays or resources

===DONE===
