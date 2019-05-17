--TEST--
pack()/unpack(): float/double tests
--FILE--
<?php
var_dump(
    'pack e',
    bin2hex(pack("e", "")),
    bin2hex(pack("e", "a")),
    bin2hex(pack("e", " ")),
    bin2hex(pack("e", NULL)),
    bin2hex(pack("e", 0)),
    bin2hex(pack("e", 1)),
    bin2hex(pack("e", 1.0)),
    bin2hex(pack("e", 10000000000000000)),
    bin2hex(pack("e", 0.591234709823149)),
    bin2hex(pack("e", 12345678901234567890.1234567898765432123456789)),
    bin2hex(pack("e", -1)),
    bin2hex(pack("e", -1.0)),
    bin2hex(pack("e", -10000000000000000)),
    bin2hex(pack("e", -0.591234709823149)),
    bin2hex(pack("e", -12345678901234567890.1234567898765432123456789)),

    'pack E',
    bin2hex(pack("E", "")),
    bin2hex(pack("E", "a")),
    bin2hex(pack("E", " ")),
    bin2hex(pack("E", NULL)),
    bin2hex(pack("E", 0)),
    bin2hex(pack("E", 1)),
    bin2hex(pack("E", 1.0)),
    bin2hex(pack("E", 10000000000000000)),
    bin2hex(pack("E", 0.591234709823149)),
    bin2hex(pack("E", 12345678901234567890.1234567898765432123456789)),
    bin2hex(pack("E", -1)),
    bin2hex(pack("E", -1.0)),
    bin2hex(pack("E", -10000000000000000)),
    bin2hex(pack("E", -0.591234709823149)),
    bin2hex(pack("E", -12345678901234567890.1234567898765432123456789)),

    'pack g',
    bin2hex(pack("g", "")),
    bin2hex(pack("g", "a")),
    bin2hex(pack("g", " ")),
    bin2hex(pack("g", NULL)),
    bin2hex(pack("g", 0)),
    bin2hex(pack("g", 1)),
    bin2hex(pack("g", 1.0)),
    bin2hex(pack("g", 10000000000000000)),
    bin2hex(pack("g", 0.591234709823149)),
    bin2hex(pack("g", 12345678901234567890.1234567898765432123456789)),
    bin2hex(pack("g", -1)),
    bin2hex(pack("g", -1.0)),
    bin2hex(pack("g", -10000000000000000)),
    bin2hex(pack("g", -0.591234709823149)),
    bin2hex(pack("g", -12345678901234567890.1234567898765432123456789)),

    'pack G',
    bin2hex(pack("G", "")),
    bin2hex(pack("G", "a")),
    bin2hex(pack("G", " ")),
    bin2hex(pack("G", NULL)),
    bin2hex(pack("G", 0)),
    bin2hex(pack("G", 1)),
    bin2hex(pack("G", 1.0)),
    bin2hex(pack("G", 10000000000000000)),
    bin2hex(pack("G", 0.591234709823149)),
    bin2hex(pack("G", 12345678901234567890.1234567898765432123456789)),
    bin2hex(pack("G", -1)),
    bin2hex(pack("G", -1.0)),
    bin2hex(pack("G", -10000000000000000)),
    bin2hex(pack("G", -0.591234709823149)),
    bin2hex(pack("G", -12345678901234567890.1234567898765432123456789)),

    'unpack e',
    unpack('e', hex2bin('0000000000000000')),
    unpack('e', hex2bin('000000000000f03f')),
    unpack('e', hex2bin('0080e03779c34143')),
    unpack('e', hex2bin('4a6ade0d65ebe23f')),
    unpack('e', hex2bin('000000000000f0bf')),
    unpack('e', hex2bin('0080e03779c341c3')),
    unpack('e', hex2bin('4a6ade0d65ebe2bf')),
    unpack('e', hex2bin('e1639d31956ae5c3')),

    'unpack E',
    unpack('E', hex2bin('3ff0000000000000')),
    unpack('E', hex2bin('4341c37937e08000')),
    unpack('E', hex2bin('3fe2eb650dde6a4a')),
    unpack('E', hex2bin('43e56a95319d63e1')),
    unpack('E', hex2bin('bff0000000000000')),
    unpack('E', hex2bin('c341c37937e08000')),
    unpack('E', hex2bin('bfe2eb650dde6a4a')),
    unpack('E', hex2bin('c3e56a95319d63e1')),

    'unpack g',
    unpack('g', hex2bin('0000803f')),
    unpack('g', hex2bin('ca1b0e5a')),
    unpack('g', hex2bin('285b173f')),
    unpack('g', hex2bin('aa542b5f')),
    unpack('g', hex2bin('000080bf')),
    unpack('g', hex2bin('ca1b0eda')),
    unpack('g', hex2bin('285b17bf')),
    unpack('g', hex2bin('aa542bdf')),

    'unpack G',
    unpack('G', hex2bin('3f800000')),
    unpack('G', hex2bin('5a0e1bca')),
    unpack('G', hex2bin('3f175b28')),
    unpack('G', hex2bin('5f2b54aa')),
    unpack('G', hex2bin('bf800000')),
    unpack('G', hex2bin('da0e1bca')),
    unpack('G', hex2bin('bf175b28')),
    unpack('G', hex2bin('df2b54aa'))

);
?>
--EXPECT--
string(6) "pack e"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "000000000000f03f"
string(16) "000000000000f03f"
string(16) "0080e03779c34143"
string(16) "4a6ade0d65ebe23f"
string(16) "e1639d31956ae543"
string(16) "000000000000f0bf"
string(16) "000000000000f0bf"
string(16) "0080e03779c341c3"
string(16) "4a6ade0d65ebe2bf"
string(16) "e1639d31956ae5c3"
string(6) "pack E"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "0000000000000000"
string(16) "3ff0000000000000"
string(16) "3ff0000000000000"
string(16) "4341c37937e08000"
string(16) "3fe2eb650dde6a4a"
string(16) "43e56a95319d63e1"
string(16) "bff0000000000000"
string(16) "bff0000000000000"
string(16) "c341c37937e08000"
string(16) "bfe2eb650dde6a4a"
string(16) "c3e56a95319d63e1"
string(6) "pack g"
string(8) "00000000"
string(8) "00000000"
string(8) "00000000"
string(8) "00000000"
string(8) "00000000"
string(8) "0000803f"
string(8) "0000803f"
string(8) "ca1b0e5a"
string(8) "285b173f"
string(8) "aa542b5f"
string(8) "000080bf"
string(8) "000080bf"
string(8) "ca1b0eda"
string(8) "285b17bf"
string(8) "aa542bdf"
string(6) "pack G"
string(8) "00000000"
string(8) "00000000"
string(8) "00000000"
string(8) "00000000"
string(8) "00000000"
string(8) "3f800000"
string(8) "3f800000"
string(8) "5a0e1bca"
string(8) "3f175b28"
string(8) "5f2b54aa"
string(8) "bf800000"
string(8) "bf800000"
string(8) "da0e1bca"
string(8) "bf175b28"
string(8) "df2b54aa"
string(8) "unpack e"
array(1) {
  [1]=>
  float(0)
}
array(1) {
  [1]=>
  float(1)
}
array(1) {
  [1]=>
  float(1.0E+16)
}
array(1) {
  [1]=>
  float(0.59123470982315)
}
array(1) {
  [1]=>
  float(-1)
}
array(1) {
  [1]=>
  float(-1.0E+16)
}
array(1) {
  [1]=>
  float(-0.59123470982315)
}
array(1) {
  [1]=>
  float(-1.2345678901235E+19)
}
string(8) "unpack E"
array(1) {
  [1]=>
  float(1)
}
array(1) {
  [1]=>
  float(1.0E+16)
}
array(1) {
  [1]=>
  float(0.59123470982315)
}
array(1) {
  [1]=>
  float(1.2345678901235E+19)
}
array(1) {
  [1]=>
  float(-1)
}
array(1) {
  [1]=>
  float(-1.0E+16)
}
array(1) {
  [1]=>
  float(-0.59123470982315)
}
array(1) {
  [1]=>
  float(-1.2345678901235E+19)
}
string(8) "unpack g"
array(1) {
  [1]=>
  float(1)
}
array(1) {
  [1]=>
  float(1.0000000272564E+16)
}
array(1) {
  [1]=>
  float(0.59123468399048)
}
array(1) {
  [1]=>
  float(1.2345679395506E+19)
}
array(1) {
  [1]=>
  float(-1)
}
array(1) {
  [1]=>
  float(-1.0000000272564E+16)
}
array(1) {
  [1]=>
  float(-0.59123468399048)
}
array(1) {
  [1]=>
  float(-1.2345679395506E+19)
}
string(8) "unpack G"
array(1) {
  [1]=>
  float(1)
}
array(1) {
  [1]=>
  float(1.0000000272564E+16)
}
array(1) {
  [1]=>
  float(0.59123468399048)
}
array(1) {
  [1]=>
  float(1.2345679395506E+19)
}
array(1) {
  [1]=>
  float(-1)
}
array(1) {
  [1]=>
  float(-1.0000000272564E+16)
}
array(1) {
  [1]=>
  float(-0.59123468399048)
}
array(1) {
  [1]=>
  float(-1.2345679395506E+19)
}
