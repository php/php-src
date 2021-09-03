--TEST--
Bug #42663 (gzinflate() try to allocate all memory with truncated $data)
--EXTENSIONS--
zlib
--FILE--
<?php
// build a predictable string
$string = '';
for($i=0; $i<30000; ++$i) $string .= $i . ' ';
var_dump(strlen($string));
// deflate string
$deflated = gzdeflate($string,9);
var_dump(strlen($deflated));
// truncate $deflated string
$truncated = substr($deflated, 0, 65535);
var_dump(strlen($truncated));
// inflate $truncated string (check if it will not eat all memory)
var_dump(gzinflate($truncated));
?>
--EXPECTF--
int(168890)
int(66743)
int(65535)

Warning: gzinflate(): data error in %s on line %d
bool(false)
