--TEST--
output buffering - handlers/status
--FILE--
<?php
function a($s){return $s;}
function b($s){return $s;}
function c($s){return $s;}
function d($s){return $s;}

ob_start();
ob_start('a');
ob_start('b');
ob_start('c');
ob_start('d');
ob_start();

echo "foo\n";

ob_flush();
ob_end_clean();
ob_flush();

print_r(ob_list_handlers());
print_r(ob_get_status());
print_r(ob_get_status(true));

?>
--EXPECTF--
foo
Array
(
    [0] => default output handler
    [1] => a
    [2] => b
    [3] => c
    [4] => d
)
Array
(
    [level] => 5
    [type] => 1
    [status] => 1
    [name] => d
    [del] => 1
)
Array
(
    [0] => Array
        (
            [chunk_size] => 0
            [size] => 40960
            [block_size] => 10240
            [type] => 1
            [status] => 0
            [name] => default output handler
            [del] => 1
        )

    [1] => Array
        (
            [chunk_size] => 0
            [size] => 40960
            [block_size] => 10240
            [type] => 1
            [status] => 0
            [name] => a
            [del] => 1
        )

    [2] => Array
        (
            [chunk_size] => 0
            [size] => 40960
            [block_size] => 10240
            [type] => 1
            [status] => 0
            [name] => b
            [del] => 1
        )

    [3] => Array
        (
            [chunk_size] => 0
            [size] => 40960
            [block_size] => 10240
            [type] => 1
            [status] => 0
            [name] => c
            [del] => 1
        )

    [4] => Array
        (
            [chunk_size] => 0
            [size] => 40960
            [block_size] => 10240
            [type] => 1
            [status] => 1
            [name] => d
            [del] => 1
        )

)