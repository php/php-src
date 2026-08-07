--TEST--
Base85 decode Adobe data missing padding strict throws UnableToDecodeException
--FILE--
<?php
use Encoding\Base85;
use function Encoding\base85_decode;

try {
    base85_decode("87cURD]j7BEbo80", variant: Base85::Adobe);
    echo "No exception thrown\n";
} catch (Encoding\UnableToDecodeException $e) {
    echo "UnableToDecodeException caught\n";
}
?>
--EXPECT--
UnableToDecodeException caught
