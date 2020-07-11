--TEST--
Test that floats are converted to string locale independently
--SKIPIF--
<?php

if (!setlocale
    (LC_ALL,
    "german", "de", "de_DE", "de_DE.ISO8859-1", "de_DE.ISO_8859-1", "de_DE.UTF-8",
    "french", "fr", "fr_FR", "fr_FR.ISO8859-1", "fr_FR.ISO_8859-1", "fr_FR.UTF-8",
    )) {
    die("skip - locale needed for this test is not supported on this platform");
}

?>
--FILE--
<?php

function print_float(float $f)
{
    echo "- casting:\n";
    echo $f . "\n";
    echo strval($f) . "\n";
    $g = $f;
    settype($g, "string");
    echo $g . "\n";

    echo "- *printf functions:\n";
    printf("%.2f\n", $f);
    printf("%.2F\n", $f);
    echo sprintf("%.2f", $f) . "\n";
    echo sprintf("%.2F", $f) . "\n";

    echo "- export/import:\n";
    echo var_export($f, true)  . "\n";
    echo serialize($f) . "\n";
    echo json_encode($f) . "\n";

    echo "- debugging:\n";
    echo print_r($f, true) . "\n";
    var_dump($f);
    debug_zval_dump($f);

    echo "- other:\n";
    echo implode([$f]) . "\n";
}

setlocale(LC_ALL, "C");
echo "C locale:\n";

print_float(3.14);

setlocale(
    LC_ALL,
    "german", "de", "de_DE", "de_DE.ISO8859-1", "de_DE.ISO_8859-1", "de_DE.UTF-8",
    "french", "fr", "fr_FR", "fr_FR.ISO8859-1", "fr_FR.ISO_8859-1", "fr_FR.UTF-8",
);
echo "\nde_DE locale:\n";

print_float(3.14);

?>
--EXPECT--
C locale:
- casting:
3.14
3.14
3.14
- *printf functions:
3.14
3.14
3.14
3.14
- export/import:
3.14
d:3.14;
3.14
- debugging:
3.14
float(3.14)
float(3.14)
- other:
3.14

de_DE locale:
- casting:
3.14
3.14
3.14
- *printf functions:
3,14
3.14
3,14
3.14
- export/import:
3.14
d:3.14;
3.14
- debugging:
3.14
float(3.14)
float(3.14)
- other:
3.14
