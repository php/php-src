--TEST--
filter_var() and FLOAT
--EXTENSIONS--
filter
--INI--
precision=14
--FILE--
<?php

$floats = array(
'1.234   ',
'   1.234',
'1.234'	,
'1.2e3',
'7E3',
'7E3     ',
'  7E3     ',
'  7E-3     '
);

foreach ($floats as $float) {
    $out = filter_var($float, FILTER_VALIDATE_FLOAT);
    var_dump($out);
}

$floats = array(
'1.234   '	=> ',',
'1,234'		=> ',',
'   1.234'	=> '.',
'1.234'		=> '..',
'1.2e3'		=> ','
);

echo "\ncustom decimal:\n";
foreach ($floats as $float => $dec) {
    try {
        var_dump(filter_var($float, FILTER_VALIDATE_FLOAT, array("options"=>array('decimal' => $dec))));
    } catch (ValueError $exception) {
        echo $exception->getMessage() . "\n";
    }
}

$floats = array(
'1.234   ',
'   1.234',
'1.234'	,
'1.2e3',
'7E3',
'7E3     ',
'  7E3     ',
'  7E-3     '
);

echo "\nno scientific:\n";
foreach ($floats as $float) {
    $out = filter_var($float, FILTER_VALIDATE_FLOAT, FILTER_FLAG_NO_SCIENTIFIC);
    var_dump($out);
}

?>
--EXPECT--
float(1.234)
float(1.234)
float(1.234)
float(1200)
float(7000)
float(7000)
float(7000)
float(0.007)

custom decimal:
bool(false)
float(1.234)
float(1.234)
filter_var(): "decimal" option must be one character long
bool(false)

no scientific:
float(1.234)
float(1.234)
float(1.234)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
