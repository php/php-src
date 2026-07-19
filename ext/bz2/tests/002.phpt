--TEST--
bzopen() using fd opened in wrong mode
--EXTENSIONS--
bz2
--FILE--
<?php

@unlink("bz_open_002.txt");

$fp = fopen("bz_open_002.txt", "w");
var_dump(bzopen($fp, "w"));

$fp = fopen("bz_open_002.txt", "r");
var_dump(bzopen($fp, "r"));

@unlink("bz_open_002.txt");
$fp = fopen("bz_open_002.txt", "x");
var_dump(bzopen($fp, "w"));

@unlink("bz_open_002.txt");
$fp = fopen("bz_open_002.txt", "x");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "rb");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "wb");
var_dump(bzopen($fp, "w"));

try {
	$fp = fopen("bz_open_002.txt", "br");
    var_dump(bzopen($fp, "r"));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

try {
	$fp = fopen("bz_open_002.txt", "br");
    var_dump(bzopen($fp, "w"));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

$fp = fopen("bz_open_002.txt", "r");
var_dump(bzopen($fp, "w"));

$fp = fopen("bz_open_002.txt", "w");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "rw");
var_dump(bzopen($fp, "w"));

$fp = fopen("bz_open_002.txt", "rw");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "wr");
var_dump(bzopen($fp, "w"));

$fp = fopen("bz_open_002.txt", "wr");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "r+");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "r+");
var_dump(bzopen($fp, "w"));

$fp = fopen("bz_open_002.txt", "w+");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "w+");
var_dump(bzopen($fp, "w"));

$fp = fopen("bz_open_002.txt", "a");
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "a");
var_dump(bzopen($fp, "w"));

@unlink("bz_open_002.txt");

?>
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)

Warning: bzopen(): Cannot read from a stream opened in write only mode in %s on line %d
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)
ValueError: fopen(): Argument #2 ($mode) must be a valid mode
ValueError: fopen(): Argument #2 ($mode) must be a valid mode

Warning: bzopen(): cannot write to a stream opened in read only mode in %s on line %d
bool(false)

Warning: bzopen(): Cannot read from a stream opened in write only mode in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'rw' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'rw' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'wr' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'wr' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'r+' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'r+' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'w+' in %s on line %d
bool(false)

Warning: bzopen(): Cannot use stream opened in mode 'w+' in %s on line %d
bool(false)

Warning: bzopen(): Cannot read from a stream opened in write only mode in %s on line %d
bool(false)
resource(%d) of type (stream)
