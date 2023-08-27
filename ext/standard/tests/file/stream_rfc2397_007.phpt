--TEST--
Stream: RFC2397 and seeking
--INI--
allow_url_fopen=1
--FILE--
<?php

$streams = array(
    "data:,012345",
    );

foreach($streams as $stream)
{
    echo "===$stream===\n";

    $fp = fopen($stream, 'rb');

    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:4,S===\n";
    var_dump(fseek($fp, 4));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===GETC===\n";
    var_dump(fgetc($fp));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===GETC===\n";
    var_dump(fgetc($fp));
    var_dump(fgetc($fp));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===REWIND===\n";
    var_dump(rewind($fp));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===GETC===\n";
    var_dump(fgetc($fp));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:3,S===\n";
    var_dump(fseek($fp, 3, SEEK_SET));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:1,C===\n";
    var_dump(fseek($fp, 1, SEEK_CUR));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:-2,C===\n";
    var_dump(fseek($fp, -2, SEEK_CUR));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:-10,C===\n";
    var_dump(fseek($fp, -10, SEEK_CUR));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:3,S===\n";
    var_dump(fseek($fp, 3, SEEK_SET));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:10,C===\n";
    var_dump(fseek($fp, 10, SEEK_CUR));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:-1,E===\n";
    var_dump(fseek($fp, -1, SEEK_END));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:0,E===\n";
    var_dump(fseek($fp, 0, SEEK_END));
    var_dump(ftell($fp));
    var_dump(feof($fp));
    echo "===S:1,E===\n";
    var_dump(fseek($fp, 1, SEEK_END));
    var_dump(ftell($fp));
    var_dump(feof($fp));

    fclose($fp);
}

?>
--EXPECT--
===data:,012345===
int(0)
bool(false)
===S:4,S===
int(0)
int(4)
bool(false)
===GETC===
string(1) "4"
int(5)
bool(false)
===GETC===
string(1) "5"
bool(false)
int(6)
bool(true)
===REWIND===
bool(true)
int(0)
bool(false)
===GETC===
string(1) "0"
int(1)
bool(false)
===S:3,S===
int(0)
int(3)
bool(false)
===S:1,C===
int(0)
int(4)
bool(false)
===S:-2,C===
int(0)
int(2)
bool(false)
===S:-10,C===
int(-1)
bool(false)
bool(false)
===S:3,S===
int(0)
int(3)
bool(false)
===S:10,C===
int(0)
int(13)
bool(false)
===S:-1,E===
int(0)
int(5)
bool(false)
===S:0,E===
int(0)
int(6)
bool(false)
===S:1,E===
int(0)
int(7)
bool(false)
