--TEST--
gzencode()/base64_encode()
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$packed=gzencode($original);
echo strlen($packed)." ".strlen($original)."\n";
if (strcmp(base64_encode($packed),"H4sIAAAAAAAAA+3GoQ0AQAgEsFV+NdwJEthf/R6kVU11z9tsRERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERETu5gPlQAe9AJAAAA==")==0) echo "Strings are equal";
?>
--EXPECT--
118 36864
Strings are equal
