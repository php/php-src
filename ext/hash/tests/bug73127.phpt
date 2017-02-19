--TEST--
Bug #73127 (gost-crypto hash incorrect if input data contains long 0xFF sequence)
--FILE--
<?php // $Id$

$test1 = str_repeat("\xFF", 40);
$test2 = str_repeat("\x00", 40);
echo hash('gost-crypto', $test1),
     "\n",
     hash('gost', $test1),
     "\n",
     hash('gost-crypto', $test2),
     "\n",
     hash('gost', $test2),
     "\n",
     hash('gost-crypto', ''),
     "\n",
     hash('gost', '')
    ;
?>
--EXPECT--
231d8bb980d3faa30fee6ec475df5669cf6c24bbce22f46d6737470043a99f8e
9eaf96ba62f90fae6707f1d4274d1a9d6680f5a121d4387815aa3a6ec42439c5
bbf1f3179409c853cd3e396d67b0e10a266c218a4fd19f149c39aa4f6d37a007
a0be0b90fea5a5b50c90c9429d07bb98fa0c06f0c30210e94c7d80c3125c67ac
981e5f3ca30c841487830f84fb433e13ac1101569b9c13584ac483234cd656c0
ce85b99cc46752fffee35cab9a7b0278abb4c2d2055cff685af4912c49490f8d
