--TEST--
gzencode()/base64_encode()
--SKIPIF--
<?php if (!extension_loaded("zlib")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
$original = str_repeat("hallo php",4096);
$packed=gzencode($original);
echo strlen($packed)." ".strlen($original)."\n";
if (strcmp(base64_encode($packed),"H4sIAAAAAAAAA+zGoQ0AQAgEsFV+NdwJEthf/R6kVU11z9tsRERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERERETu5gMAAP//AwDlQAe9AJAAAA==")==0) echo "Strings are equal";
?>
--EXPECT--
124 36864
Strings are equal
