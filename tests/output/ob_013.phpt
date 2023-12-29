--TEST--
output buffering - handlers/status
--FILE--
<?php
function a($s){return $s;}
function b($s){return $s;}
function c($s){return $s;}
function d($s){return $s;}

class E {
    public static function f($s) {
        return $s;
    }

    public function g($s) {
        return $s;
    }

    public function __invoke($s) {
        return $s;
    }
}

$e = new E();

ob_start();
ob_start('a');
ob_start('b');
ob_start('c');
ob_start('d');
ob_start(['E', 'f']);
ob_start([$e, 'g']);
ob_start($e);
ob_start(function ($s) { return $s; });
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
    [5] => E::f
    [6] => E::g
    [7] => E::__invoke
    [8] => Closure::__invoke
)
Array
(
    [name] => Closure::__invoke
    [type] => 1
    [flags] => 20593
    [level] => 8
    [chunk_size] => 0
    [buffer_size] => 16384
    [buffer_used] => %d
)
Array
(
    [0] => Array
        (
            [name] => default output handler
            [type] => 0
            [flags] => 112
            [level] => 0
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [1] => Array
        (
            [name] => a
            [type] => 1
            [flags] => 113
            [level] => 1
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [2] => Array
        (
            [name] => b
            [type] => 1
            [flags] => 113
            [level] => 2
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [3] => Array
        (
            [name] => c
            [type] => 1
            [flags] => 113
            [level] => 3
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [4] => Array
        (
            [name] => d
            [type] => 1
            [flags] => 113
            [level] => 4
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [5] => Array
        (
            [name] => E::f
            [type] => 1
            [flags] => 113
            [level] => 5
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [6] => Array
        (
            [name] => E::g
            [type] => 1
            [flags] => 113
            [level] => 6
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => 0
        )

    [7] => Array
        (
            [name] => E::__invoke
            [type] => 1
            [flags] => 113
            [level] => 7
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => %d
        )

    [8] => Array
        (
            [name] => Closure::__invoke
            [type] => 1
            [flags] => 20593
            [level] => 8
            [chunk_size] => 0
            [buffer_size] => 16384
            [buffer_used] => %d
        )

)
