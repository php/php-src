--TEST--
Str\Matcher: overlapping keywords are all counted and listed
--EXTENSIONS--
strmatch
--FILE--
<?php
$m = new Str\Matcher(['a', 'aa', 'aaa']);
var_dump($m->countMatches('aaaaa')); // 5x a + 4x aa + 3x aaa = 12
foreach ($m->matchAll('aaa') as $r) {
    printf("%d:%d:%s\n", $r->offset, $r->length, $r->keyword);
}
?>
--EXPECT--
int(12)
0:1:a
0:2:aa
0:3:aaa
1:1:a
1:2:aa
2:1:a
