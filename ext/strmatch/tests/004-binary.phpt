--TEST--
Str matching is byte-based: UTF-8, embedded NUL, high bytes and the full alphabet
--EXTENSIONS--
strmatch
--FILE--
<?php
$m = new Str\Matcher(['日本', 'мир', 'niño']);
var_dump($m->countMatches('日本と日本、мир niño'));
foreach ($m->matchAll('x日本y') as $r) {
    printf("%d:%d:%s\n", $r->offset, $r->length, $r->keyword);
}
$r = new Str\Replacer(['мир' => 'peace']);
var_dump($r->replace('мир миру мир'));
$m = new Str\Matcher(["a\0b", "\xff\xfe"]);
var_dump($m->countMatches("x\xff\xfey a\0b z a\0b"));
var_dump($m->matches("a\0b"));

$matcher = new Str\Matcher(["\x97"]);
$hit = $matcher->matchAll("\xe6\x97\xa5")[0];
var_dump($hit->offset, $hit->length, bin2hex($hit->keyword));
var_dump(bin2hex((new Str\Replacer(["\x97" => 'X']))->replace("\xe6\x97\xa5")));
$keywords = [];
for ($i = 0; $i < 256; $i++) {
    $keywords[] = chr($i) . chr(255 - $i);
}
$matcher = new Str\Matcher($keywords);
$subject = '';
for ($i = 0; $i < 512; $i++) {
    $subject .= chr(($i * 37 + 11) % 256);
}
$subject .= $keywords[0] . $keywords[128] . $keywords[255] . $keywords[7];
$unique = [];
$count = 0;
foreach ($keywords as $keyword) {
    if (str_contains($subject, $keyword)) {
        $unique[] = $keyword;
    }
    $offset = 0;
    while (($position = strpos($subject, $keyword, $offset)) !== false) {
        $count++;
        $offset = $position + 1;
    }
}
sort($unique);
$actual = $matcher->getMatchedKeywords($subject);
sort($actual);
var_dump($actual === $unique, $matcher->countMatches($subject) === $count);
$map = array_fill_keys($keywords, 'R');
var_dump((new Str\Replacer($map))->replace($subject) === strtr($subject, $map));
?>
--EXPECT--
int(4)
1:6:日本
string(19) "peace peaceу peace"
int(3)
bool(true)
int(1)
int(1)
string(2) "97"
string(6) "e658a5"
bool(true)
bool(true)
bool(true)
