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
    [name] => d
    [type] => 1
    [flags] => 20593
    [level] => 4
    [chunk_size] => %d
    [buffer_size] => 16384
    [buffer_used] => 96
)
Array
(
    [0] => Array
        (
            [name] => default output handler
            [type] => 0
            [flags] => 112
            [level] => 0
            [chunk_size] => %d
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [1] => Array
        (
            [name] => a
            [type] => 1
            [flags] => 113
            [level] => 1
            [chunk_size] => %d
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [2] => Array
        (
            [name] => b
            [type] => 1
            [flags] => 113
            [level] => 2
            [chunk_size] => %d
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [3] => Array
        (
            [name] => c
            [type] => 1
            [flags] => 113
            [level] => 3
            [chunk_size] => %d
            [buffer_size] => 16384
            [buffer_used] => 4
        )

    [4] => Array
        (
            [name] => d
            [type] => 1
            [flags] => 20593
            [level] => 4
            [chunk_size] => %d
            [buffer_size] => 16384
            [buffer_used] => %d
        )

)
