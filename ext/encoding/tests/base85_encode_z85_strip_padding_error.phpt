--TEST--
Base85 encode Z85 with StripPadding throws ValueError
--FILE--
<?php
use Encoding\Base85;
use Encoding\PaddingMode;
use function Encoding\base85_encode;

try {
    base85_encode('Hello world!', variant: Base85::Z85, paddingMode: PaddingMode::StripPadding);
    echo "No exception thrown\n";
} catch (ValueError $e) {
    echo "ValueError caught\n";
}
?>
--EXPECT--
ValueError caught
