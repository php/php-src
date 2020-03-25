--TEST--
Bug #31213 (Sideeffects caused by bug #29493)
--FILE--
<?php
function test($use_extract) {
    $a = 1;
    $b = 1;

    $arr = array(
        '_a' => $a,
        '_b' => &$b
    );

    var_dump($a, $b);

    if ($use_extract) {
        extract($arr, EXTR_REFS);
    } else {
        $_a = &$arr['_a'];
        $_b = &$arr['_b'];
    }

    $_a++;
    $_b++;

    var_dump($a, $b, $_a, $_b, $arr);
}

test(false);
test(true);

?>
--EXPECT--
int(1)
int(1)
int(1)
int(2)
int(2)
int(2)
array(2) {
  ["_a"]=>
  &int(2)
  ["_b"]=>
  &int(2)
}
int(1)
int(1)
int(1)
int(2)
int(2)
int(2)
array(2) {
  ["_a"]=>
  &int(2)
  ["_b"]=>
  &int(2)
}
