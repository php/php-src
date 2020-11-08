--TEST--
Test mb_ereg() function : usage variations - pass different character classes to see they match correctly
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg') or die("skip mb_ereg() is not available in this build");
version_compare(MB_ONIGURUMA_VERSION, '6.1.0', '>=') or die("skip requires oniguruma >= 6.1.0");
?>
--FILE--
<?php
/*
 * test that mb_ereg can match correctly when passed different character classes.
 */

echo "*** Testing mb_ereg() : variation ***\n";


mb_regex_encoding('utf-8'); // have to set otherwise won't match $mb properly
$mb = base64_decode('5pel5pys6Kqe');
$character_classes = array ('aB1'    => '[[:alnum:]]+', /*1*/
                            'aBcD'   => '[[:alpha:]]+',
                            'ab/='   => '[[:ascii:]]+',
                            " \t"    => '[[:blank:]]+',
                            '234'    => '[[:digit:]]+', /*5*/
                            "$mb"    => '[[:graph:]]+',
                            'fjds'   => '[[:lower:]]+',
                            "$mb\t"  => '[[:print:]]+',
                            '.!"*@'  => '[[:punct:]]+',
                            "\t"     => '[[:space:]]+', /*10*/
                            'IDSJV'  => '[[:upper:]]+',
                            '3b5D'   => '[[:xdigit:]]+'); /*12*/

$iterator = 1;
foreach($character_classes as $string => $pattern) {
    if (is_array(@$regs)) {
        $regs = null;
    }
    // make sure any multibyte output is in base 64
    echo "\n-- Iteration $iterator --\n";
    var_dump(mb_ereg($pattern, $string, $regs));
    base64_encode_var_dump($regs);
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
--EXPECT--
*** Testing mb_ereg() : variation ***

-- Iteration 1 --
bool(true)
array(1) {
  [0]=>
  string(4) "YUIx"
}

-- Iteration 2 --
bool(true)
array(1) {
  [0]=>
  string(8) "YUJjRA=="
}

-- Iteration 3 --
bool(true)
array(1) {
  [0]=>
  string(8) "YWIvPQ=="
}

-- Iteration 4 --
bool(true)
array(1) {
  [0]=>
  string(4) "IAk="
}

-- Iteration 5 --
bool(true)
array(1) {
  [0]=>
  string(4) "MjM0"
}

-- Iteration 6 --
bool(true)
array(1) {
  [0]=>
  string(12) "5pel5pys6Kqe"
}

-- Iteration 7 --
bool(true)
array(1) {
  [0]=>
  string(8) "Zmpkcw=="
}

-- Iteration 8 --
bool(true)
array(1) {
  [0]=>
  string(12) "5pel5pys6Kqe"
}

-- Iteration 9 --
bool(true)
array(1) {
  [0]=>
  string(8) "LiEiKkA="
}

-- Iteration 10 --
bool(true)
array(1) {
  [0]=>
  string(4) "CQ=="
}

-- Iteration 11 --
bool(true)
array(1) {
  [0]=>
  string(8) "SURTSlY="
}

-- Iteration 12 --
bool(true)
array(1) {
  [0]=>
  string(8) "M2I1RA=="
}
Done
