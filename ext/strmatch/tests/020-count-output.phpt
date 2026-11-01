--TEST--
Str\Replacer output count: aliasing and typed references with matches, no matches and empty inputs
--EXTENSIONS--
strmatch
--FILE--
<?php
declare(strict_types=1);

$r = new Str\Replacer(['x' => 'Y']);
foreach ([null, 'before', [], false] as $count) {
    var_dump($r->replace('xx', $count), $count);
}
$subject = str_repeat('x', 64);
var_dump($r->replace($subject, $subject) === str_repeat('Y', 64), $subject);

$box = new class { public string $count = 'before'; };
foreach ([[$r, 'xx'], [$r, 'absent'], [$r, ''], [new Str\Replacer([]), 'xx']] as [$replacer, $subject]) {
    try {
        $replacer->replace($subject, $box->count);
        echo "FAIL\n";
    } catch (TypeError $e) {
        echo $e::class, "\n";
    }
    var_dump($box->count);
}
?>
--EXPECT--
string(2) "YY"
int(2)
string(2) "YY"
int(2)
string(2) "YY"
int(2)
string(2) "YY"
int(2)
bool(true)
int(64)
TypeError
string(6) "before"
TypeError
string(6) "before"
TypeError
string(6) "before"
TypeError
string(6) "before"
