--TEST--
iptcembed() and wrong file
--FILE--
<?php

$file = __DIR__.'/iptcembed_001.data';
$fp = fopen($file, "w");
fwrite($fp, "-1-1");
fclose($fp);

var_dump(iptcembed(-1, $file, -1));
unlink($file);

echo "Done\n";
?>
--EXPECT--
bool(false)
Done	
