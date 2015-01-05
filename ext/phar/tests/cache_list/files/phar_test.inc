<?php

if (function_exists('date_default_timezone_set')) {
	date_default_timezone_set('UTC');
}

$manifest = (binary)'';
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
	$meta = NULL;

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
	if (empty($crc32))$crc32= crc32((binary)$cont);
	if (isset($meta)) $meta = serialize($meta);

	// write manifest entry
	$manifest .= pack('V', strlen($name)) . (binary)$name;
	$manifest .= pack('VVVVVV', $ulen, $time, $clen, $crc32, $flags|$perm, strlen($meta)) . (binary)$meta;

	// globals
	$gflags |= $flags;
	$files[$name] = $comp;
}

if (!isset($alias)) $alias = 'hio';

if (isset($pmeta)) $pmeta = serialize($pmeta); else $pmeta = '';

$manifest = pack('VnVV', count($files), isset($hasdir) ? 0x1110 : 0x1000, $gflags, strlen($alias)) . (binary)$alias . pack('V', strlen($pmeta)) . (binary)$pmeta . $manifest;
$file = (binary)$file;
$file .= pack('V', strlen($manifest)) . $manifest;

foreach($files as $cont)
{
	$file .= (binary)$cont;
}

file_put_contents($fname, $file);

if (@$gzip) {
	$fp = gzopen($fname, 'w');
	fwrite($fp, $file);
	fclose($fp);
}

if (@$bz2) {
	$fp = bzopen($fname, 'w');
	fwrite($fp, $file);
	fclose($fp);
}

?>