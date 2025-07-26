--TEST--
octdec(): basic
--FILE--
<?php

$values = [
    0o567 => '567',
    0o17777777777 => '17777777777',
];

$values_leading = [
    0o567 => '0567',
    0o17777777777 => '017777777777',
];

$values_leading_explicit = [
    0o567 => '0o567',
    0o17777777777 => '0o17777777777',
];

foreach ($values as $nb => $value) {
    echo "octdec('$value') should be $nb is ";
    $res = octdec($value);
    var_dump($res);
}

foreach ($values_leading as $nb => $value) {
    echo "octdec('$value') should be $nb is ";
    $res = octdec($value);
    var_dump($res);
}

foreach ($values_leading_explicit as $nb => $value) {
    echo "octdec('$value') should be $nb is ";
    $res = octdec($value);
    var_dump($res);
}

?>
--EXPECT--
octdec('567') should be 375 is int(375)
octdec('17777777777') should be 2147483647 is int(2147483647)
octdec('0567') should be 375 is int(375)
octdec('017777777777') should be 2147483647 is int(2147483647)
octdec('0o567') should be 375 is int(375)
octdec('0o17777777777') should be 2147483647 is int(2147483647)
