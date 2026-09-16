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
	'file://evil.example.com/x',
];
$wrappers = [
	'',
	'compress.zlib://',
	'compress.zlib://compress.zlib://',
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
bool(true)

compress.zlib://compress.zlib://http://127.0.0.1/example.html
bool(true)

ftp://127.0.0.1/example.html
bool(false)

compress.zlib://ftp://127.0.0.1/example.html
bool(true)

compress.zlib://compress.zlib://ftp://127.0.0.1/example.html
bool(true)

/etc/os-release
bool(true)

compress.zlib:///etc/os-release
bool(true)

compress.zlib://compress.zlib:///etc/os-release
bool(true)

file://evil.example.com/x
bool(false)

compress.zlib://file://evil.example.com/x
bool(true)

compress.zlib://compress.zlib://file://evil.example.com/x
bool(true)
