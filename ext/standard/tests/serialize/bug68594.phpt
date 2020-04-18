--TEST--
Bug #68545 Use after free vulnerability in unserialize()
--FILE--
<?php
for ($i=4; $i<100; $i++) {
    $m = new StdClass();

    $u = array(1);

    $m->aaa = array(1,2,&$u,4,5);
    $m->bbb = 1;
    $m->ccc = &$u;
    $m->ddd = str_repeat("A", $i);

    $z = serialize($m);
    $z = str_replace("bbb", "aaa", $z);
    $y = unserialize($z);
    $z = serialize($y);
}
?>
===DONE===
--EXPECT--
===DONE===
