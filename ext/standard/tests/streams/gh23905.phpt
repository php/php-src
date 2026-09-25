--TEST--
GH-23905 (Failed seek on php://memory and SQLite blob streams sets the stream position to -1)
--FILE--
<?php
foreach (['php://memory', 'php://temp'] as $url) {
    echo $url, "\n";
    $stream = fopen($url, 'r+');
    fwrite($stream, 'hello world');

    fseek($stream, 6);
    var_dump(fseek($stream, -7, SEEK_CUR));
    var_dump(ftell($stream));
    var_dump(fseek($stream, 1, SEEK_CUR));
    var_dump(ftell($stream), fread($stream, 4));

    var_dump(fseek($stream, -12, SEEK_END));
    var_dump(ftell($stream));
    var_dump(fseek($stream, -5, SEEK_CUR));
    var_dump(ftell($stream), fread($stream, 5));

    fclose($stream);
}
?>
--EXPECT--
php://memory
int(-1)
int(6)
int(0)
int(7)
string(4) "orld"
int(-1)
int(11)
int(0)
int(6)
string(5) "world"
php://temp
int(-1)
int(6)
int(0)
int(7)
string(4) "orld"
int(-1)
int(11)
int(0)
int(6)
string(5) "world"
