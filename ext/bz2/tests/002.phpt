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

$fp = fopen("bz_open_002.txt", "br");
try {
    var_dump(bzopen($fp, "r"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$fp = fopen("bz_open_002.txt", "br");
try {
    var_dump(bzopen($fp, "w"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
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

Warning: fopen(bz_open_002.txt): Failed to open stream: `br' is not a valid mode for fopen in %s on line %d
bzopen(): Argument #1 ($file) must be of type string or file-resource, false given

Warning: fopen(bz_open_002.txt): Failed to open stream: `br' is not a valid mode for fopen in %s on line %d
bzopen(): Argument #1 ($file) must be of type string or file-resource, false given

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
