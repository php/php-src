--TEST--
Html\render_component: body content routes to the #[Html\Slot] parameter (RFC §5)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;
use Html\Element as E;
use Html\Fragment;
use Html\Slot;

// The body slot routes to the #[Slot] parameter; other content areas are
// ordinary props typed Html\Htmlable (what named slots used to be).
function Layout(
    string $lang,
    #[Slot] Html\Htmlable $body,
    ?Html\Htmlable $header = null,
    ?Html\Htmlable $footer = null,
): Html\Htmlable {
    return new E('main', ['lang' => $lang], [$header, $body, $footer]);
}

$out = render_component(
    'Layout',
    [
        'lang'   => 'en',
        'header' => new E('h1', [], ['Title']),
        'footer' => new Fragment([new E('small', [], ['(c) 2026'])]),
    ],
    new Fragment([new E('p', [], ['loose body'])]),          // body slot
);
echo $out, "\n";

// A component on a class constructor, body slot only.
class Panel implements Html\Htmlable {
    public function __construct(
        public string $kind,
        #[Slot] public ?Html\Htmlable $content = null,
    ) {}
    public function toHtml(): Html\Htmlable {
        return new E('section', ['class' => $this->kind], [$this->content]);
    }
}
echo render_component(Panel::class, ['kind' => 'note'], new Fragment([Html\raw('<b>hi</b>')]))->__toString(), "\n";
?>
--EXPECT--
<main lang="en"><h1>Title</h1><p>loose body</p><small>(c) 2026</small></main>
<section class="note"><b>hi</b></section>
