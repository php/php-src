--TEST--
Base85 decode forgiving mode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base85;
use Encoding\DecodingMode;

use function Encoding\base85_decode;

var_dump(base85_decode("87cURD]j7BEbo80", variant: Base85::Adobe, decodingMode: DecodingMode::Forgiving));

?>
--EXPECT--
string(12) "Hello world!"
