--TEST--
Bug #34214 (base64_decode() does not properly ignore whitespace)
--FILE--
<?php
$txt = 'Zm9vYmFyIG
 Zvb2Jhcg==';
echo base64_decode($txt), "\n";
?>
--EXPECT--
foobar foobar
