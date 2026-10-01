--TEST--
Test mb_ereg() function : usage variations - pass different character classes as pattern for multibyte string
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
?>
--FILE--
<?php
/*
 * Test how character classes match a multibyte string
 */

echo "*** Testing mb_ereg() : usage variations ***\n";

mb_regex_encoding('utf-8');

//contains japanese characters, ASCII digits and different, UTF-8 encoded digits
$string_mb = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII=');

$character_classes = array ('[[:alnum:]]+', /*1*/
                            '[[:alpha:]]+',
                            '[[:ascii:]]+',
                            '[[:blank:]]+',
                            '[[:cntrl:]]+',/*5*/
                            '[[:digit:]]+',
                            '[[:graph:]]+',
                            '[[:lower:]]+',
                            '[[:print:]]+',
                            '[[:punct:]]+', /*10*/
                            '[[:space:]]+',
                            '[[:upper:]]+',
                            '[[:xdigit:]]+'); /*13*/

$iterator = 1;
foreach ($character_classes as $pattern) {
    if (is_array(@$regs)) {
        $regs = null;
    }
    echo "\n-- Iteration $iterator --\n";
    var_dump(mb_ereg($pattern, $string_mb, $regs));
    if ($regs) {
        base64_encode_var_dump($regs);
    }
    $iterator++;
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
--EXPECTF--
*** Testing mb_ereg() : usage variations ***

Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d

-- Iteration 1 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(64) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZMDEyMzTvvJXvvJbvvJfvvJjvvJk="
}

-- Iteration 2 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(36) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ"
}

-- Iteration 3 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(8) "MDEyMzQ="
}

-- Iteration 4 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(false)

-- Iteration 5 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(false)

-- Iteration 6 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(28) "MDEyMzTvvJXvvJbvvJfvvJjvvJk="
}

-- Iteration 7 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(68) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}

-- Iteration 8 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(false)

-- Iteration 9 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(68) "5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZMDEyMzTvvJXvvJbvvJfvvJjvvJnjgII="
}

-- Iteration 10 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(4) "44CC"
}

-- Iteration 11 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(false)

-- Iteration 12 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(false)

-- Iteration 13 --

Deprecated: Function mb_ereg() is deprecated since 8.6, because the underlying library is no longer maintained in %s line %d
bool(true)
array(1) {
  [0]=>
  string(8) "MDEyMzQ="
}
Done
