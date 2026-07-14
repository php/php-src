--TEST--
Markup syntax: the `#lazy` directive defers slot evaluation via Markup\LazyFragment
--EXTENSIONS--
markup
--FILE--
<?php
use Markup\Element as E;

// An Auth component that renders its body only when "logged in". With `#lazy`
// the body is never evaluated when discarded.
class Auth implements Markup\Html {
    public function __construct(public bool $check, #[Markup\Slot] public ?Markup\Html $slot = null) {}
    public function toHtml(): Markup\Html {
        return $this->check ? new E('div', [], [$this->slot]) : new Markup\Fragment([]);
    }
}

$evaluated = 0;
function track(): string { global $evaluated; $evaluated++; return 'secret'; }

// Logged out: the body interpolation must never run.
echo <Auth check={false} #lazy>Hello {track()}</Auth>, "\n";
var_dump($evaluated); // 0 - never evaluated

// Logged in: the body evaluates exactly once, even though a component could
// in principle render its slot twice (LazyFragment memoizes).
class Twice implements Markup\Html {
    public function __construct(#[Markup\Slot] public Markup\Html $slot) {}
    public function toHtml(): Markup\Html {
        return new Markup\Fragment([$this->slot, $this->slot]);
    }
}
$evaluated = 0;
echo <Twice #lazy>x{track()}</Twice>, "\n";
var_dump($evaluated); // 1 - memoized across both renders

// Without `#lazy` the body is eager: track() runs before Auth even sees it.
$evaluated = 0;
echo <Auth check={false}>Hello {track()}</Auth>, "\n";
var_dump($evaluated); // 1 - evaluated eagerly

// The lowered value is a Markup\LazyFragment that is itself a Markup\Html.
$lazy = new Markup\LazyFragment(fn() => new E('span', [], ['built']));
var_dump($lazy instanceof Markup\Html);
var_dump($lazy->toHtml() instanceof Markup\Element);
echo $lazy, "\n";

// The thunk must return a Markup\Html.
try {
    (new Markup\LazyFragment(fn() => 'nope'))->toHtml();
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
Markup\LazyFragment thunk must return a Markup\Html
