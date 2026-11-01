--TEST--
Str API: final classes, immutable snapshots, private result construction and readonly fields
--EXTENSIONS--
strmatch
--FILE--
<?php
foreach ([Str\Matcher::class, Str\Replacer::class, Str\MatchResult::class] as $class) {
    $reflection = new ReflectionClass($class);
    var_dump($reflection->isFinal(), $reflection->getParentClass());
}
$value = 'before';
$map = [-1 => 'negative', 123 => &$value, '01' => 'leading', "a\0b" => 'binary'];
$r = new Str\Replacer($map);
var_dump($r->getReplacements() === $map);
$value = 'after';
$snapshot = $r->getReplacements();
var_dump($snapshot[123]);
$snapshot[123] = 'changed';
var_dump($r->replace('123'));
var_dump(method_exists($r, 'matches'), method_exists($r, 'getKeywords'));
try {
    new Str\MatchResult();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
foreach ([new Str\Matcher([]), new Str\Replacer([])] as $object) {
    try {
        $object->extra = 1;
    } catch (Error $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}

$result = (new Str\Matcher(['ab', 'bc']))->matchAll('zabc')[0];
printf("%d %d %s\n", $result->offset, $result->length, $result->keyword);
foreach (['offset', 'length', 'keyword'] as $property) {
    try {
        $result->$property = $result->$property;
        echo "FAIL\n";
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
string(6) "before"
string(6) "before"
bool(false)
bool(false)
Error: Call to private Str\MatchResult::__construct() from global scope
Error: Cannot create dynamic property Str\Matcher::$extra
Error: Cannot create dynamic property Str\Replacer::$extra
1 2 ab
Error: Cannot modify readonly property Str\MatchResult::$offset
Error: Cannot modify readonly property Str\MatchResult::$length
Error: Cannot modify readonly property Str\MatchResult::$keyword
