--TEST--
Str replacement: independent longest-match oracle, suffix priority, integer keys and aliasing
--EXTENSIONS--
strmatch
--FILE--
<?php
function oracle(string $subject, array $map): array {
    $output = '';
    $count = 0;
    for ($position = 0; $position < strlen($subject);) {
        $best = null;
        foreach ($map as $keyword => $replacement) {
            $keyword = (string) $keyword;
            if (substr_compare($subject, $keyword, $position, strlen($keyword)) === 0
                && ($best === null || strlen($keyword) > strlen($best))) {
                $best = $keyword;
            }
        }
        if ($best === null) {
            $output .= $subject[$position++];
        } else {
            $output .= $map[$best];
            $position += strlen($best);
            $count++;
        }
    }
    return [$output, $count];
}
$maps = [[], ['a' => '', 'aa' => 'a', 'aaa' => 'Z'], ['ab' => 'b', 'bc' => 'c', 'abc' => 'Y'], ['x' => 'X', 'xy' => 'Z', 'y' => 'x'], ["a\0" => '', "\0b" => 'a', "\xff" => "\0"]];
$subjects = ['', 'q', 'a', 'aaaaaaa', 'abcabc', 'xxyxy', "a\0b\xffa\0"];
$cases = 0;
foreach ($maps as $map) {
    $replacer = new Str\Replacer($map);
    foreach ($subjects as $subject) {
        $expected = oracle($subject, $map);
        $actual = [$replacer->replace($subject, $count), $count];
        if ($actual !== $expected || $actual[0] !== strtr($subject, $map)) {
            var_dump($map, $subject, $expected, $actual);
            exit;
        }
        $cases++;
    }
}
echo $cases, " cases OK\n";
$subject = str_repeat('x', 64);
var_dump((new Str\Replacer([]))->replace($subject, $subject) === str_repeat('x', 64), $subject);
echo (new Str\Replacer(['cab' => 'LONG', 'a' => 'SHORT']))->replace('ab', $count), "\n";
var_dump($count);
echo (new Str\Replacer(['ab' => 'X', 'bcde' => 'Y']))->replace('abcde', $count), "\n";
var_dump($count);

$map = [-1 => 'X', 123 => 'Y', 'ab' => 'Z'];
$replacer = new Str\Replacer($map);
var_dump($replacer->replace('q-1 123 ab', $count), $count);
var_dump($replacer->getReplacements() === $map);

$map = ['a' => 'A', 'aa' => 'Q', 'aba' => 'X'];
$chunk = str_repeat('x', 253) . 'aba';
$subjects = [str_repeat($chunk, 300), str_repeat('a', 2000) . str_repeat($chunk, 300)];
foreach ($subjects as $subject) {
    $replacer = new Str\Replacer($map);
    if ([$replacer->replace($subject, $count), $count] !== oracle($subject, $map)) {
        throw new Exception('Long-subject replacement mismatch');
    }
}
echo "Long subjects OK\n";
?>
--EXPECT--
35 cases OK
bool(true)
int(0)
SHORTb
int(1)
Xcde
int(1)
string(6) "qX Y Z"
int(3)
bool(true)
Long subjects OK
