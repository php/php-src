--TEST--
Base85 encode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base85;
use Encoding\PaddingMode;

use function Encoding\base85_encode;

$data = 'Hello world!';

echo base85_encode($data, variant: Base85::Adobe);
echo "\n";

echo base85_encode($data, variant: Base85::Adobe, paddingMode: PaddingMode::StripPadding);
echo "\n";

?>
--EXPECT--
<~87cURD]j7BEbo80~>
87cURD]j7BEbo80
