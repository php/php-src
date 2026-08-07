--TEST--
Base85 encode Adobe variant
--FILE--
<?php
use Encoding\Base85;
use function Encoding\base85_encode;

echo base85_encode('Hello world!', variant: Base85::Adobe);
echo "\n";
?>
--EXPECT--
<~87cURD]j7BEbo80~>
