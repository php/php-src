--TEST--
GH-12908 (Show attribute name/class in ReflectionAttribute dump)
--FILE--
<?php

#[\AllowDynamicProperties, \Foo]
class Test {
}

print_r((new ReflectionClass(Test::class))->getAttributes());

?>
--EXPECT--
Array
(
    [0] => ReflectionAttribute Object
        (
            [name] => AllowDynamicProperties
        )

    [1] => ReflectionAttribute Object
        (
            [name] => Foo
        )

)
