--TEST--
Testing stream_is_local() with zlib wrappers
--EXTENSIONS--
zlib
--FILE--
<?php

$targets = [
	'http://127.0.0.1/example.html',
	'ftp://127.0.0.1/example.html',
	'/etc/os-release',
];
$wrappers = [
	'',
	'compress.zlib://',
	'zlib:',
];

foreach ($targets as $target) {
	foreach ($wrappers as $wrapper) {
		$case = $wrapper . $target;
		echo $case . "\n";
		var_dump(stream_is_local($case));
		echo "\n";
	}
}

?>
--EXPECT--
http://127.0.0.1/example.html
bool(false)

compress.zlib://http://127.0.0.1/example.html
bool(false)

zlib:http://127.0.0.1/example.html
bool(false)

ftp://127.0.0.1/example.html
bool(false)

compress.zlib://ftp://127.0.0.1/example.html
bool(false)

zlib:ftp://127.0.0.1/example.html
bool(false)

/etc/os-release
bool(true)

compress.zlib:///etc/os-release
bool(true)

zlib:/etc/os-release
bool(true)
