<?php

$manifest = '';

foreach($files as $name => $cont)
{
	$ulen = strlen($cont);
	$clen = $ulen;
	$time = isset($ftime) ? $ftime : @mktime(12, 0, 0, 3, 1, 2006);
	$manifest .= pack('V', strlen($name)) . $name;
	$manifest .= pack('VVVVV', $ulen, $time, $clen, crc32($cont), 0x000001B6);
}
$alias = 'hio';
$manifest = pack('VnVV', count($files), 0x0900, 0x00000000, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents($fname, $file);

?>