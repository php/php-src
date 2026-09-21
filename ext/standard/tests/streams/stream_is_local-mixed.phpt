--TEST--
Testing stream_is_local() with bz2 and zlib wrappers
--EXTENSIONS--
bz2
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
	'compress.bzip2://compress.zlib://',
	'compress.zlib://compress.bzip2://',
	str_repeat("compress.zlib://compress.bzip2://", 20),
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
--EXPECTF--
http://127.0.0.1/example.html
bool(false)

compress.bzip2://compress.zlib://http://127.0.0.1/example.html
bool(false)

compress.zlib://compress.bzip2://http://127.0.0.1/example.html
bool(false)

%r(compress\.zlib:\/\/compress\.bzip2:\/\/){20}%rhttp://127.0.0.1/example.html
bool(false)

ftp://127.0.0.1/example.html
bool(false)

compress.bzip2://compress.zlib://ftp://127.0.0.1/example.html
bool(false)

compress.zlib://compress.bzip2://ftp://127.0.0.1/example.html
bool(false)

%r(compress\.zlib:\/\/compress\.bzip2:\/\/){20}%rftp://127.0.0.1/example.html
bool(false)

/etc/os-release
bool(true)

compress.bzip2://compress.zlib:///etc/os-release
bool(true)

compress.zlib://compress.bzip2:///etc/os-release
bool(true)

%r(compress\.zlib:\/\/compress\.bzip2:\/\/){20}%r/etc/os-release
bool(true)

file://evil.example.com/x
bool(false)

compress.bzip2://compress.zlib://file://evil.example.com/x
bool(true)

compress.zlib://compress.bzip2://file://evil.example.com/x
bool(true)

%r(compress\.zlib:\/\/compress\.bzip2:\/\/){20}%rfile://evil.example.com/x
bool(true)
