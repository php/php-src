--TEST--
bzopen() using fd opened in wrong mode
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
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
var_dump(bzopen($fp, "r"));

$fp = fopen("bz_open_002.txt", "br");
var_dump(bzopen($fp, "w"));

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

echo "Done\n";
?>
--EXPECTF--
resource(%d) of type (stream)
resource(%d) of type (stream)
resource(%d) of type (stream)

Warning: bzopen(): cannot read from a stream opened in write only mode in %s on line %d
bool(false)
resource(%d) of type (stream)
resource(%d) of type (stream)

Warning: fopen(bz_open_002.txt): failed to open stream: Bad file %s in %s on line %d

Warning: bzopen(): first parameter has to be string or file-resource in %s on line %d
bool(false)

Warning: fopen(bz_open_002.txt): failed to open stream: Bad file %s in %s on line %d

Warning: bzopen(): first parameter has to be string or file-resource in %s on line %d
bool(false)

Warning: bzopen(): cannot write to a stream opened in read only mode in %s on line %d
bool(false)

Warning: bzopen(): cannot read from a stream opened in write only mode in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'rw' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'rw' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'wr' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'wr' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'r+' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'r+' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'w+' in %s on line %d
bool(false)

Warning: bzopen(): cannot use stream opened in mode 'w+' in %s on line %d
bool(false)

Warning: bzopen(): cannot read from a stream opened in write only mode in %s on line %d
bool(false)
resource(%d) of type (stream)
Done
