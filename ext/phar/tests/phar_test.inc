<?php

$manifest = '';
$glags = 0;

foreach($files as $name => $cont)
{
	global $gflags, $files;

	$comp = NULL;
	$crc32= NULL;
	$clen = NULL;
	$ulen = NULL;
	$time = isset($ftime) ? $ftime : @mktime(12, 0, 0, 3, 1, 2006);
	$flags= 0;
	$perm = 0x000001B6;
	$meta = array();

	// overwrite if array
	if (is_array($cont))
	{
		foreach(array('comp','crc32','clen','ulen','time','flags','perm','meta','cont') as $what)
		{
			if (isset($cont[$what]))
			{
				$$what = $cont[$what];
			}
		}
	}

	// create if not yet done
	if (empty($comp)) $comp = $cont;
	if (empty($ulen)) $ulen = strlen($cont);
	if (empty($clen)) $clen = strlen($comp);
	if (empty($crc32))$crc32= crc32($cont);

	// write manifest entry
	$manifest .= pack('V', strlen($name)) . $name;
	$manifest .= pack('VVVVV', $ulen, $time, $clen, $crc32, $flags|$perm);
	foreach($meta as $type => $data)
	{
		$manifest .= pack('Vv', $type, strlen($data)) . $data;
	}
	$manifest .= pack('V', 0);

	// globals
	$gflags |= $flags;
	$files[$name] = $comp;
}

$alias = 'hio';
$manifest = pack('VnVV', count($files), 0x0900, $glags, strlen($alias)) . $alias . $manifest;
$file .= pack('V', strlen($manifest)) . $manifest;

foreach($files as $cont)
{
	$file .= $cont;
}

file_put_contents($fname, $file);

?>