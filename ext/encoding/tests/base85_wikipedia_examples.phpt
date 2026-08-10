--TEST--
Base85 Wikipedia examples (Adobe Ascii85)
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base85;
use Encoding\PaddingMode;

use function Encoding\base85_encode;
use function Encoding\base85_decode;

// Example from Wikipedia: "Man " -> "9jqo^"
echo base85_encode("Man ", variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo "\n";

// Example from Wikipedia: "sure" -> "F*2M7"
echo base85_encode("sure", variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo "\n";

// The full Hobbes quote from Wikipedia - check encoding length
$hobbesQuote = 'Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.';
$encoded = base85_encode($hobbesQuote, variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo strlen($hobbesQuote), "\n";
echo strlen($encoded), "\n";

// Verify "z" shorthand for four zero bytes
$zeros = "\x00\x00\x00\x00";
echo base85_encode($zeros, variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo "\n";

// Decode "z" back to four zero bytes
$decodedZeros = base85_decode("z", variant: Base85::Adobe);
var_dump($decodedZeros === $zeros);

// Verify encoding of "Hello world!" with delimiters
echo base85_encode("Hello world!", variant: Base85::Adobe);
echo "\n";

// Decode "Hello world!" without delimiters
echo base85_encode("Hello world!", variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo "\n";
$decoded = base85_decode("87cURD]j7BEbo80", variant: Base85::Adobe);
var_dump($decoded === "Hello world!");

?>
--EXPECT--
9jqo^
F*2M7
269
336
z
bool(true)
<~87cURD]j7BEbo80~>
87cURD]j7BEbo80
bool(true)
