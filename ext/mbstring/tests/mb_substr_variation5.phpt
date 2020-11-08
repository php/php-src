--TEST--
Test mb_substr() function : usage variations - pass different integers to $length arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr') or die("skip mb_substr() is not available in this build");
?>
--FILE--
<?php
/*
 * Test how mb_substr() behaves when passed a range of integers as $length argument
 */

echo "*** Testing mb_substr() : usage variations ***\n";

mb_internal_encoding('UTF-8');

$string_ascii = '+Is an English string'; //21 chars

//Japanese string, 21 characters
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

/*
 * Loop through integers as multiples of ten for $offset argument
 * 60 is larger than *BYTE* count for $string_mb
 */
for ($i = -60; $i <= 60; $i += 10) {
    if (@$a || @$b) {
        $a = null;
        $b = null;
    }
    echo "\n**-- Offset is: $i --**\n";
    echo "-- ASCII String --\n";
    $a = mb_substr($string_ascii, 1, $i);
    var_dump(base64_encode($a));
    echo "--Multibyte String --\n";
    $b = mb_substr($string_mb, 1, $i, 'UTF-8');
    if (strlen($a) == mb_strlen($b, 'UTF-8')) { // should return same length
        var_dump(base64_encode($b));
    } else {
        echo "Difference in length of ASCII string and multibyte string\n";
    }

}

echo "Done";
?>
--EXPECT--
*** Testing mb_substr() : usage variations ***

**-- Offset is: -60 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -50 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -40 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -30 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -20 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: -10 --**
-- ASCII String --
string(16) "SXMgYW4gRW5nbA=="
--Multibyte String --
string(40) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMA=="

**-- Offset is: 0 --**
-- ASCII String --
string(0) ""
--Multibyte String --
string(0) ""

**-- Offset is: 10 --**
-- ASCII String --
string(16) "SXMgYW4gRW5nbA=="
--Multibyte String --
string(40) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMA=="

**-- Offset is: 20 --**
-- ASCII String --
string(28) "SXMgYW4gRW5nbGlzaCBzdHJpbmc="
--Multibyte String --
string(68) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="

**-- Offset is: 30 --**
-- ASCII String --
string(28) "SXMgYW4gRW5nbGlzaCBzdHJpbmc="
--Multibyte String --
string(68) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="

**-- Offset is: 40 --**
-- ASCII String --
string(28) "SXMgYW4gRW5nbGlzaCBzdHJpbmc="
--Multibyte String --
string(68) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="

**-- Offset is: 50 --**
-- ASCII String --
string(28) "SXMgYW4gRW5nbGlzaCBzdHJpbmc="
--Multibyte String --
string(68) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="

**-- Offset is: 60 --**
-- ASCII String --
string(28) "SXMgYW4gRW5nbGlzaCBzdHJpbmc="
--Multibyte String --
string(68) "5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
Done
