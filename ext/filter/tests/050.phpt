--TEST--
filter_var() and double overflow/underflow
--EXTENSIONS--
filter
--FILE--
<?php
$test = array(
    '1e+308'					=> 1e+308,
    '1e+309'					=> false,
    '1e-323'					=> 1e-323,
    '1e-324'					=> false,
);
foreach ($test as $src => $dst) {
    $out = filter_var($src, FILTER_VALIDATE_FLOAT);
    if ($dst !== $out) {
        if ($out === false) {
            echo "$src -> false != $dst\n";
        } elseif ($dst === false) {
            echo "$src -> $out != false\n";
        } else {
            echo "$src -> $out != $dst\n";
        }
    }
}

echo "Ok\n";
?>
--EXPECT--
Ok
