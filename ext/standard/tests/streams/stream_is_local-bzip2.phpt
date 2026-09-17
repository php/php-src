--TEST--
Testing stream_is_local() with bzip2 wrappers
--EXTENSIONS--
bz2
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
	'compress.bzip2://',
	'compress.bzip2://compress.bzip2://',
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

compress.bzip2://http://127.0.0.1/example.html
bool(false)

compress.bzip2://compress.bzip2://http://127.0.0.1/example.html
bool(false)

ftp://127.0.0.1/example.html
bool(false)

compress.bzip2://ftp://127.0.0.1/example.html
bool(false)

compress.bzip2://compress.bzip2://ftp://127.0.0.1/example.html
bool(false)

/etc/os-release
bool(true)

compress.bzip2:///etc/os-release
bool(true)

compress.bzip2://compress.bzip2:///etc/os-release
bool(true)

file://evil.example.com/x
bool(false)

compress.bzip2://file://evil.example.com/x
bool(true)

compress.bzip2://compress.bzip2://file://evil.example.com/x
bool(true)
