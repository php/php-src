--TEST--
Native markup: bare "<" after operand-ending tokens stays a comparison (no-space BC)
--FILE--
<?php
// Postfix ++/-- end an operand: "<" is a comparison, not markup.
$i = 0; $n = 5;
$a = [1, 2, 3];
var_dump($i++<PHP_INT_MAX);
var_dump($i--<$n);
$count = 0;
while ($count++<count($a)) {}
var_dump($count);

// The `class` keyword in ::class ends an operand too.
const FOO = "zzz";
var_dump(\stdClass::class<FOO);
var_dump(self_name()<FOO);
function self_name(): string { return "aaa"; }

// Plain constants / qualified names end an operand.
var_dump(FOO<"zzzz");
var_dump(PHP_INT_MAX<FOO);

// `]` (array dereference) and `}` end an operand.
var_dump($a[0]<FOO);
var_dump(match(true) { default => 1 }<FOO);

// A closing heredoc label ends an operand.
var_dump(<<<EOT
aaa
EOT<FOO);

// Interpolated string closing quote ends an operand.
$s = "aa";
var_dump("a$s"<FOO);

// `static` is a complete operand (instanceof static), never markup.
class Base { public function check(object $o): bool { return $o instanceof static<FOO; } }
var_dump((new Base)->check(new Base));

// Bare `exit` is an expression operand; "<" after it is a comparison.
// (Only reached when the left side is falsy-compared, so guard it.)
function never_exit(): bool { return false && exit<FOO; }
var_dump(never_exit());

// The legacy `<>` operator is untouched in infix (operator) position.
var_dump($a[0]<>2);

// Plain variables, literals, and `)` end an operand too, with or without spaces.
$lhs = 1; $rhs = 2;
var_dump($lhs < $rhs);
var_dump($lhs<$rhs);
var_dump(3 < 4);
var_dump(($lhs) < $rhs);

// Other "<"-led operators are unchanged: spaceship, less-or-equal, shifts.
var_dump($lhs <=> $rhs);
var_dump($lhs <= $rhs);
var_dump(1 << 4);
var_dump(64 >> 2);

// A heredoc body containing "<" stays literal text, not markup.
echo <<<EOT
literal <not-markup> text
EOT, "\n";

// Error suppression with "@" still works before an expression.
$sup = []; var_dump(@$sup['missing']);

// Bare `yield` is a complete operand: `<` and `<>` after it stay comparisons
// ((yield) != 5, (yield) < FOO), with or without spaces. Yielding markup
// therefore needs parentheses: `yield (<div/>)`.
function gen_ne() { var_dump(yield <> 5); }
$g = gen_ne(); $g->current(); $g->send(7);
function gen_lt() { var_dump(yield < FOO); }
$g = gen_lt(); $g->current(); $g->send("aaa");
function gen_lt_nospace() { $r = yield<FOO; var_dump($r); }
$g = gen_lt_nospace(); $g->current(); $g->send("aaa");

// Inverse direction: in operand position (after `=`, `(`, `,`, `=>`, `return`,
// and parenthesized after `yield`) a "<tag" IS markup. Compile-time
// classification only - never executed, so this parses even in builds where
// the Html\ runtime classes are unavailable.
function never_runs() {
    $x = <div/>;
    take(<p>hi</p>, <br/>);
    $f = fn() => <span>a</span>;
    yield (<li>item</li>);
    return <em>done</em>;
}

// Dynamic tags: "<$tag" and "<{expr}" in operand position are markup
// ("$i--<$n" above is the operator-position inverse, still a comparison).
function never_runs_dynamic() {
    $tag = 'div';
    $x = <$tag class="a">hi</$tag>;
    $y = <{$tag . 'x'}>hi</>;
    take(<{pick()}/>);
    return <$tag/>;
}
echo "parsed\n";
?>
--EXPECT--
bool(true)
bool(true)
int(4)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(-1)
bool(true)
int(16)
int(16)
literal <not-markup> text
NULL
bool(true)
bool(true)
bool(true)
parsed
