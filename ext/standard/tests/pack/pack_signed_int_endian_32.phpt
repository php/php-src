--TEST--
pack()/unpack(): signed integer endianness tests (32-bit)
--SKIPIF--
<?php
if (PHP_INT_SIZE > 4) {
    die("skip 32bit test only");
}
?>
--FILE--
<?php
var_dump(
    // pack little endian signed 2-byte
    bin2hex(pack("m", 0)),
    bin2hex(pack("m", 1)),
    bin2hex(pack("m", -1)),
    bin2hex(pack("m", 32767)),
    bin2hex(pack("m", -32768)),

    // pack big endian signed 2-byte
    bin2hex(pack("y", 0)),
    bin2hex(pack("y", 1)),
    bin2hex(pack("y", -1)),
    bin2hex(pack("y", 32767)),
    bin2hex(pack("y", -32768)),

    // pack little endian signed 4-byte
    bin2hex(pack("M", 0)),
    bin2hex(pack("M", 1)),
    bin2hex(pack("M", -1)),
    bin2hex(pack("M", 2147483647)),
    bin2hex(pack("M", -2147483648)),

    // pack big endian signed 4-byte
    bin2hex(pack("Y", 0)),
    bin2hex(pack("Y", 1)),
    bin2hex(pack("Y", -1)),
    bin2hex(pack("Y", 2147483647)),
    bin2hex(pack("Y", -2147483648)),

    // unpack little endian signed 2-byte
    unpack('m', hex2bin('0000')),
    unpack('m', hex2bin('0100')),
    unpack('m', hex2bin('ffff')),
    unpack('m', hex2bin('ff7f')),
    unpack('m', hex2bin('0080')),

    // unpack big endian signed 2-byte
    unpack('y', hex2bin('0000')),
    unpack('y', hex2bin('0001')),
    unpack('y', hex2bin('ffff')),
    unpack('y', hex2bin('7fff')),
    unpack('y', hex2bin('8000')),

    // unpack little endian signed 4-byte
    unpack('M', hex2bin('00000000')),
    unpack('M', hex2bin('01000000')),
    unpack('M', hex2bin('ffffffff')),
    unpack('M', hex2bin('ffffff7f')),
    unpack('M', hex2bin('00000080')),

    // unpack big endian signed 4-byte
    unpack('Y', hex2bin('00000000')),
    unpack('Y', hex2bin('00000001')),
    unpack('Y', hex2bin('ffffffff')),
    unpack('Y', hex2bin('7fffffff')),
    unpack('Y', hex2bin('80000000'))
);

// Test 8-byte formats throw errors on 32-bit
try {
    pack("p", 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    pack("j", 0);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    unpack("p", hex2bin('0000000000000000'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    unpack("j", hex2bin('0000000000000000'));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
string(4) "0000"
string(4) "0100"
string(4) "ffff"
string(4) "ff7f"
string(4) "0080"
string(4) "0000"
string(4) "0001"
string(4) "ffff"
string(4) "7fff"
string(4) "8000"
string(8) "00000000"
string(8) "01000000"
string(8) "ffffffff"
string(8) "ffffff7f"
string(8) "00000080"
string(8) "00000000"
string(8) "00000001"
string(8) "ffffffff"
string(8) "7fffffff"
string(8) "80000000"
array(1) {
  [1]=>
  int(0)
}
array(1) {
  [1]=>
  int(1)
}
array(1) {
  [1]=>
  int(-1)
}
array(1) {
  [1]=>
  int(32767)
}
array(1) {
  [1]=>
  int(-32768)
}
array(1) {
  [1]=>
  int(0)
}
array(1) {
  [1]=>
  int(1)
}
array(1) {
  [1]=>
  int(-1)
}
array(1) {
  [1]=>
  int(32767)
}
array(1) {
  [1]=>
  int(-32768)
}
array(1) {
  [1]=>
  int(0)
}
array(1) {
  [1]=>
  int(1)
}
array(1) {
  [1]=>
  int(-1)
}
array(1) {
  [1]=>
  int(2147483647)
}
array(1) {
  [1]=>
  int(-2147483648)
}
array(1) {
  [1]=>
  int(0)
}
array(1) {
  [1]=>
  int(1)
}
array(1) {
  [1]=>
  int(-1)
}
array(1) {
  [1]=>
  int(2147483647)
}
array(1) {
  [1]=>
  int(-2147483648)
}
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
64-bit format codes are not available for 32-bit versions of PHP
