--TEST--
Base85 encode ValueError
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base85;
use Encoding\PaddingMode;

use function Encoding\base85_encode;

$data = 'Hello world!';

try {
    base85_encode($data, variant: Base85::Z85, paddingMode: PaddingMode::StripPadding);
    echo "No exception\n";
} catch (ValueError $e) {
    echo $e::class, "\n";
}

?>
--EXPECTF--
ValueError
