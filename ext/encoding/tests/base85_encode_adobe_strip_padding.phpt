--TEST--
Base85 encode Adobe with StripPadding
--FILE--
<?php
use Encoding\Base85;
use Encoding\PaddingMode;
use function Encoding\base85_encode;

echo base85_encode('Hello world!', variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo "\n";
?>
--EXPECT--
87cURD]j7BEbo80
