--TEST--
retrieving aggregation info
--POST--
--GET--
--FILE--
<?php
include "./ext/standard/tests/aggregation/aggregate.lib";

$obj = new simple();
aggregate($obj, 'mixin');
print_r(aggregation_info($obj));
?>
--EXPECT--
I'm alive!
Array
(
    [mixin] => Array
        (
            [methods] => Array
                (
                    [0] => mix_it
                )

            [properties] => Array
                (
                    [0] => mix
                )

        )

)
