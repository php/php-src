--TEST--
Base85 decode Z85 data with Adobe variant
--FILE--
<?php
use Encoding\Base85;
use function Encoding\base85_decode;

$encodedZ85 = "nm=QNz.92Pz/P";

try {
    $result = base85_decode($encodedZ85, variant: Base85::Adobe);
    // Z85 data decoded with Adobe alphabet - may return meaningless data
    echo "Result: $result\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECTF--
%
