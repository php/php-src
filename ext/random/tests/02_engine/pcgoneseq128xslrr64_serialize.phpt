--TEST--
Random: Engine: PcgOneseq128XslRr64: serialize
--FILE--
<?php

echo serialize(new \Random\Engine\PcgOneseq128XslRr64(1234));

?>
--EXPECT--
O:33:"Random\Engine\PcgOneseq128XslRr64":2:{i:0;a:0:{}i:1;a:2:{i:0;s:16:"c6d571c37c41a8d1";i:1;s:16:"345e7e82265d6e27";}}
