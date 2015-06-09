--TEST--
Bug #68710 Use after free vulnerability in unserialize() (bypassing the
CVE-2014-8142 fix)
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
    $z = str_replace("aaa", "123", $z);
    $z = str_replace("bbb", "123", $z);
    $y = unserialize($z);
    $z = serialize($y);
}
?>
===DONE===
--EXPECTF--
===DONE===
