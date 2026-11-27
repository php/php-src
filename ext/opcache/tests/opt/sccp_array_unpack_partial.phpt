--TEST--
SCCP must not infer numeric keys when unpacking into a partial array
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.optimization_level=-1
opcache.jit=0
--EXTENSIONS--
opcache
--FILE--
<?php
function test($unknown) {
    $array = [$unknown, ...[42]];
    var_dump($array[0], $array[1]);

    $array = [10, $unknown, ...[42, 43]];
    var_dump($array[0], $array[1], $array[2], $array[3]);

    $array = [100 => $unknown, ...[42]];
    var_dump($array[0] ?? 'missing', $array[100], $array[101]);

    $array = [$unknown, 'key' => 0, ...[42, 'key' => 43], 44];
    var_dump($array[0], $array[1], $array[2], $array['key']);
}
test(7);
?>
--EXPECT--
int(7)
int(42)
int(10)
int(7)
int(42)
int(43)
string(7) "missing"
int(7)
int(42)
int(7)
int(42)
int(44)
int(43)
