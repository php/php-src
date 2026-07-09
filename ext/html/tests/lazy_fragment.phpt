--TEST--
Markup syntax: the `:lazy` directive defers slot evaluation via Html\LazyFragment (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;

// An Auth component that renders its body only when "logged in". With `:lazy`
// the body is never evaluated when discarded.
class Auth implements Html\Htmlable {
    public function __construct(public bool $check, #[Html\Slot] public ?Html\Htmlable $slot = null) {}
    public function toHtml(): Html\Htmlable {
        return $this->check ? new E('div', [], [$this->slot]) : new Html\Fragment([]);
    }
}

$evaluated = 0;
function track(): string { global $evaluated; $evaluated++; return 'secret'; }

// Logged out: the body interpolation must never run.
echo <Auth check={false} :lazy>Hello {track()}</Auth>, "\n";
var_dump($evaluated); // 0 - never evaluated

// Logged in: the body evaluates exactly once, even though a component could
// in principle render its slot twice (LazyFragment memoizes).
class Twice implements Html\Htmlable {
    public function __construct(#[Html\Slot] public Html\Htmlable $slot) {}
    public function toHtml(): Html\Htmlable {
        return new Html\Fragment([$this->slot, $this->slot]);
    }
}
$evaluated = 0;
echo <Twice :lazy>x{track()}</Twice>, "\n";
var_dump($evaluated); // 1 - memoized across both renders

// Without `:lazy` the body is eager: track() runs before Auth even sees it.
$evaluated = 0;
echo <Auth check={false}>Hello {track()}</Auth>, "\n";
var_dump($evaluated); // 1 - evaluated eagerly

// The lowered value is an Html\LazyFragment that is itself an Html\Htmlable.
$lazy = new Html\LazyFragment(fn() => new E('span', [], ['built']));
var_dump($lazy instanceof Html\Htmlable);
var_dump($lazy->toHtml() instanceof Html\Element);
echo $lazy, "\n";

// The thunk must return an Html\Htmlable.
try {
    (new Html\LazyFragment(fn() => 'nope'))->toHtml();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--

int(0)
xsecretxsecret
int(1)

int(1)
bool(true)
bool(true)
<span>built</span>
Html\LazyFragment thunk must return an Html\Htmlable
