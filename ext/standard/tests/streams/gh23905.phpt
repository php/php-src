--TEST--
GH-23905 (Failed seek on php://memory and php://temp streams sets the stream position to -1)
--FILE--
<?php
foreach ([
    'php://memory',
    'php://filter/string.rot13/resource=php://memory',
    'php://temp',
    'php://temp/maxmemory:0',
    'php://filter/string.rot13/resource=php://temp',
    'php://filter/string.rot13/resource=php://temp/maxmemory:0',
] as $url) {
    echo $url, "\n";
    $stream = fopen($url, 'r+');
    fwrite($stream, 'hello world');

    echo "-- SEEK_SET --\n";
    fseek($stream, 6);
    var_dump(fseek($stream, -1, SEEK_SET), ftell($stream));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), fread($stream, 5));

    var_dump(fseek($stream, 15, SEEK_SET), feof($stream), ftell($stream));
    var_dump(fseek($stream, -15, SEEK_CUR), ftell($stream), fread($stream, 15), feof($stream));

    echo "-- SEEK_CUR --\n";
    fseek($stream, 6);
    var_dump(fseek($stream, -7, SEEK_CUR), ftell($stream));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), fread($stream, 5));

    var_dump(fseek($stream, 10, SEEK_CUR), feof($stream), ftell($stream));
    var_dump(fseek($stream, -15, SEEK_CUR), ftell($stream), fread($stream, 15), feof($stream));

    echo "-- SEEK_END --\n";
    fseek($stream, 6);
    var_dump(fseek($stream, -12, SEEK_END), ftell($stream));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), fread($stream, 5));

    var_dump(fseek($stream, 4, SEEK_END), feof($stream), ftell($stream));
    var_dump(fseek($stream, -15, SEEK_CUR), ftell($stream), fread($stream, 15), feof($stream));

    fclose($stream);
    echo "\n\n";
}
?>
--EXPECT--
php://memory
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)


php://filter/string.rot13/resource=php://memory
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)


php://temp
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)


php://temp/maxmemory:0
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)


php://filter/string.rot13/resource=php://temp
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)


php://filter/string.rot13/resource=php://temp/maxmemory:0
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(0)
bool(false)
int(15)
int(0)
int(0)
string(11) "hello world"
bool(true)
