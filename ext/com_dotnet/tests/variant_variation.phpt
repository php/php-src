--TEST--
Testing variant arrays
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$arrays = [
    "order" => [2 => 1, 1 => 2, 0 => 3],
    "off" => [2 => 1, 1 => 2, 3],
    "negative" => [-1 => 42],
];
foreach ($arrays as $desc => $array) {
    echo "-- $desc --\n";
    $v = new variant($array);
    foreach ($v as $val) {
        var_dump($val);
    }
}
?>
--EXPECTF--
-- order --
int(3)
int(2)
int(1)
-- off --
NULL
int(2)
int(1)
-- negative --
%ANULL
