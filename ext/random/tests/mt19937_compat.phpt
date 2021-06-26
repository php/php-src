--TEST--
Test Random: MT19937 compatibility
--FILE--
<?php

$seed = random_int(PHP_INT_MIN, PHP_INT_MAX);
mt_srand($seed);
$random = new Random(new Random\NumberGenerator\MT19937($seed));

for ($i = 0; $i < 20000; $i++) {
    $mt_rand_ret = mt_rand();
    $random_ret = $random->nextInt();

    if ($mt_rand_ret !== $random_ret) {
        die("failure: mt_rand_ret: ${mt_rand_ret} random_ret: ${random_ret} i: ${i}");
    }
}

if ($random->shuffleString('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.') !==
    str_shuffle('Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.')
) {
    die("faiilure: shuffleString / str_shuffle");
}

$arr_shuffle_random_ret = $random->shuffleArray(range(1, 1000));
$arr_shuffle_mt_rand_ret = range(1, 1000); shuffle($arr_shuffle_mt_rand_ret);
if (
    serialize($arr_shuffle_random_ret) !==
    serialize($arr_shuffle_mt_rand_ret)
) {
    die("failure: shuffleArray / shuffle");
}

die('success');
?>
--EXPECT--
success
