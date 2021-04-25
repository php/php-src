--TEST--
Fileinfo run upstream tests
--EXTENSIONS--
fileinfo
--ENV--
TZ=UTC
--FILE--
<?php

$lst = glob(__DIR__ . DIRECTORY_SEPARATOR . "upstream/*.testfile");

foreach($lst as $p) {
	$mp = dirname($p) . DIRECTORY_SEPARATOR . basename($p, ".testfile") . ".magic";
	$tp = dirname($p) . DIRECTORY_SEPARATOR . basename($p, ".testfile") . ".result";
	$finfo = finfo_open(FILEINFO_NONE, file_exists($mp) ? $mp : NULL);
	$i = finfo_file( $finfo, $p);
	$exp = file_get_contents($tp);
	if ($i !== $exp) {
		echo "'$p' failed\nexp: '$exp'\ngot: '$i'\n";
	} 
	finfo_close($finfo);
}

echo "==DONE==";
?>
--EXPECT--
==DONE==
