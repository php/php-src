--TEST--
Str replacement: independent longest-match oracle, suffix priority and integer keys
--FILE--
<?php
function oracle(string $subject, array $map): string {
    $output = '';
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
        }
    }
    return $output;
}
$maps = [[], ['a' => '', 'aa' => 'a', 'aaa' => 'Z'], ['ab' => 'b', 'bc' => 'c', 'abc' => 'Y'], ['x' => 'X', 'xy' => 'Z', 'y' => 'x'], ["a\0" => '', "\0b" => 'a', "\xff" => "\0"]];
$subjects = ['', 'q', 'a', 'aaaaaaa', 'abcabc', 'xxyxy', "a\0b\xffa\0"];
$cases = 0;
foreach ($maps as $map) {
    $replacer = new Str\Replacer($map);
    foreach ($subjects as $subject) {
        $expected = oracle($subject, $map);
        $actual = $replacer->replace($subject);
        if ($actual !== $expected || $actual !== strtr($subject, $map)) {
            var_dump($map, $subject, $expected, $actual);
            exit;
        }
        $cases++;
    }
}
echo $cases, " cases OK\n";
$subject = str_repeat('x', 64);
var_dump((new Str\Replacer([]))->replace($subject) === $subject);
echo (new Str\Replacer(['cab' => 'LONG', 'a' => 'SHORT']))->replace('ab'), "\n";
echo (new Str\Replacer(['ab' => 'X', 'bcde' => 'Y']))->replace('abcde'), "\n";

$map = [-1 => 'X', 123 => 'Y', 'ab' => 'Z'];
$replacer = new Str\Replacer($map);
var_dump($replacer->replace('q-1 123 ab'));
var_dump($replacer->getReplacements() === $map);

$map = ['a' => 'A', 'aa' => 'Q', 'aba' => 'X'];
$chunk = str_repeat('x', 253) . 'aba';
$subjects = [str_repeat($chunk, 300), str_repeat('a', 2000) . str_repeat($chunk, 300)];
foreach ($subjects as $subject) {
    $replacer = new Str\Replacer($map);
    if ($replacer->replace($subject) !== oracle($subject, $map)) {
        throw new Exception('Long-subject replacement mismatch');
    }
}
echo "Long subjects OK\n";
?>
--EXPECT--
35 cases OK
bool(true)
SHORTb
Xcde
string(6) "qX Y Z"
bool(true)
Long subjects OK
