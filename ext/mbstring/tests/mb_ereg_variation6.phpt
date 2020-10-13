--TEST--
Test mb_ereg() function : usage variations - match special characters
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/*
 * Test how mb_ereg() matches special characters for $pattern
 */

echo "*** Testing mb_ereg() : usage variations ***\n";

if(mb_regex_encoding('utf-8') == true) {
    echo "Regex encoding set to utf-8\n";
} else {
    echo "Could not set regex encoding to utf-8\n";
}

$regex_char = array ('\w+' => '\w+',
                     '\W+' => '\W+',
                     '\s+' => '\s+',
                     '\S+' => '\S+',
                     '\d+' => '\d+',
                     '\D+' => '\D+',
                     '\b' =>  '\b',
                     '\B' =>  '\B');

$string_ascii = 'This is an English string. 0123456789.';
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

foreach ($regex_char as $displayChar => $char) {
    echo "\n--** Pattern is: $displayChar **--\n";
    if (@$regs_ascii || @$regs_mb) {
        $regs_ascii = null;
        $regs_mb = null;
    }
    echo "-- ASCII String: --\n";
    var_dump(mb_ereg($char, $string_ascii, $regs_ascii));
    base64_encode_var_dump($regs_ascii);

    echo "-- Multibyte String: --\n";
    var_dump(mb_ereg($char, $string_mb, $regs_mb));
    base64_encode_var_dump($regs_mb);

}

/**
 * replicate a var dump of an array but outputted string values are base64 encoded
 *
 * @param array $regs
 */
function base64_encode_var_dump($regs) {
    if ($regs) {
        echo "array(" . count($regs) . ") {\n";
        foreach ($regs as $key => $value) {
            echo "  [$key]=>\n  ";
            if (is_string($value)) {
                var_dump(base64_encode($value));
            } else {
                var_dump($value);
            }
        }
        echo "}\n";
    } else {
        echo "NULL\n";
    }
}

echo "Done";

?>
--EXPECT--
*** Testing mb_ereg() : usage variations ***
Regex encoding set to utf-8

--** Pattern is: \w+ **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  string(8) "VGhpcw=="
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  string(36) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ"
}

--** Pattern is: \W+ **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  string(4) "IA=="
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  string(4) "44CC"
}

--** Pattern is: \s+ **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  string(4) "IA=="
}
-- Multibyte String: --
bool(false)
NULL

--** Pattern is: \S+ **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  string(8) "VGhpcw=="
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  string(72) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CCMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}

--** Pattern is: \d+ **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  string(16) "MDEyMzQ1Njc4OQ=="
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  string(28) "MDEyMzTvvJXvvJbvvJfvvJjvvJk="
}

--** Pattern is: \D+ **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  string(36) "VGhpcyBpcyBhbiBFbmdsaXNoIHN0cmluZy4g"
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  string(40) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC"
}

--** Pattern is: \b **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  bool(false)
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  bool(false)
}

--** Pattern is: \B **--
-- ASCII String: --
bool(true)
array(1) {
  [0]=>
  bool(false)
}
-- Multibyte String: --
bool(true)
array(1) {
  [0]=>
  bool(false)
}
Done
