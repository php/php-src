--TEST--
octdec(): invalid chars
--FILE--
<?php

$values = [
    -0o567 => '-567',
    0o177 => '1779',
    0o177 => '177+j',
];

foreach ($values as $nb => $value) {
    echo "octdec('$value') should be $nb is";
    $res = octdec($value);
    var_dump($res);
}

?>
--EXPECTF--
octdec('-567') should be -375 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(375)
octdec('177+j') should be 127 is
Deprecated: Invalid characters passed for attempted conversion, these have been ignored in %s on line %d
int(127)
