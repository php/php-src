--TEST--
date_get_last_errors() output all the error messages
--FILE--
<?php
DateTime::createFromFormat('Y-n-j a g:i:s', '2018-11-8 am 9:57:43');
print_r(date_get_last_errors());
?>
--EXPECT--
Array
(
    [warning_count] => 0
    [warnings] => Array
        (
        )

    [error_count] => 2
    [errors] => Array
        (
            [0] => Array
                 (
                     [0] => Meridian can only come after an hour has been found
                     [1] => Unexpected data found.
                 )

        )

)