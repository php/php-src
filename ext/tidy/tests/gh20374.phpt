--TEST--
GH-20374 (PHP with tidy and custom-tags)
--EXTENSIONS--
tidy
--CREDITS--
franck-paul
--FILE--
<?php

class MyStringable {
	public function __construct(private $ret) {}

	public function __toString(): string {
		return $this->ret;
	}
}

class MyThrowingStringable {
    public function __toString(): string {
        throw new Error('no');
    }
}

$values = [
	'string blocklevel' => 'blocklevel',
	'int' => 1,
	'double overflow' => (string) (2.0**80.0),
	'numeric string int 1' => '1',
	'numeric string double 1.0' => '1.0',
	'false' => false,
	'true' => true,
	'NAN' => NAN,
	'INF' => INF,
	'object with numeric string int 0' => new MyStringable('0'),
	'object with string blocklevel' => new MyStringable('blocklevel'),
    'object with string empty' => new MyStringable('empty'),
    'object with exception' => new MyThrowingStringable,
];

foreach ($values as $key => $value) {
	echo "--- $key ---\n";
	$str = '<custom-html-element>test</custom-html-element>';

	$config = [
		'custom-tags' => $value,
	];

	$tidy = new tidy();
    try {
        $tidy->parseString($str, $config, 'utf8');
        echo $tidy->value, "\n";
    } catch (Throwable $e) {
        echo $e::class, ": ", $e->getMessage(), "\n";
    }
}

?>
--EXPECTF--
--- string blocklevel ---
<html>
<head>
<title></title>
</head>
<body>
<custom-html-element>test</custom-html-element>
</body>
</html>
--- int ---
<html>
<head>
<title></title>
</head>
<body>
<custom-html-element>test</custom-html-element>
</body>
</html>
--- double overflow ---
<html>
<head>
<title></title>
</head>
<body>
test
</body>
</html>
--- numeric string int 1 ---
<html>
<head>
<title></title>
</head>
<body>
<custom-html-element>test</custom-html-element>
</body>
</html>
--- numeric string double 1.0 ---
<html>
<head>
<title></title>
</head>
<body>
<custom-html-element>test</custom-html-element>
</body>
</html>
--- false ---
<html>
<head>
<title></title>
</head>
<body>
test
</body>
</html>
--- true ---
<html>
<head>
<title></title>
</head>
<body>
<custom-html-element>test</custom-html-element>
</body>
</html>
--- NAN ---

Warning: The float NAN is not representable as an int, cast occurred in %s on line %d
<html>
<head>
<title></title>
</head>
<body>
test
</body>
</html>
--- INF ---

Warning: The float INF is not representable as an int, cast occurred in %s on line %d
<html>
<head>
<title></title>
</head>
<body>
test
</body>
</html>
--- object with numeric string int 0 ---
<html>
<head>
<title></title>
</head>
<body>
test
</body>
</html>
--- object with string blocklevel ---
<html>
<head>
<title></title>
</head>
<body>
<custom-html-element>test</custom-html-element>
</body>
</html>
--- object with string empty ---
<custom-html-element>
<html>
<head>
<title></title>
</head>
<body>
test
</body>
</html>
--- object with exception ---
Error: no
