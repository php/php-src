--TEST--
deaggreating
--POST--
--GET--
--FILE--
<?php
include dirname(__FILE__) . "/aggregate.lib";

$obj = new simple();
aggregate($obj, 'helper');
aggregate($obj, 'mixin');
print_r(aggregation_info($obj));
deaggregate($obj, 'helper');
print_r(aggregation_info($obj));
deaggregate($obj);
var_dump(aggregation_info($obj));
?>
--EXPECT--
I'm alive!
Array
(
    [helper] => Array
        (
            [methods] => Array
                (
                    [0] => do_this
                    [1] => do_that
                    [2] => just_another_method
                )

            [properties] => Array
                (
                    [0] => my_prop
                    [1] => your_prop
                    [2] => our_prop
                )

        )

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
bool(false)
