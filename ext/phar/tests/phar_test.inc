<?php

$manifest = '';

foreach($files as $name => $cont)
{
	$time = isset($ftime) ? $ftime : @mktime(12, 0, 0, 3, 1, 2006);
	$perm = 0x000001B6;
	$meta = array();
	if (is_array($cont))
	{
		foreach(array('time','perm','meta','cont') as $what)
		{
			if (isset($cont[$what]))
			{
				$$what = $cont[$what];
			}
		}
	}
	$ulen = strlen($cont);
	$clen = $ulen;
	$manifest .= pack('V', strlen($name)) . $name;
	$manifest .= pack('VVVVV', $ulen, $time, $clen, crc32($cont), $perm);
	foreach($meta as $type => $data)
	{
		$manifest .= pack('Vv', $type, strlen($data)) . $data;
	}
	$manifest .= pack('V', 0);
}
$alias = 'hio';
$manifest = pack('VnVV', count($files), 0x0900, 0x00000000, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;
foreach($files as $cont)
{
	$file .= is_array($cont) ? $cont['cont'] : $cont;
}

file_put_contents($fname, $file);

?>