--TEST--
Html\render_component: slot routing to #[Html\Slot] parameters (RFC §6)
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;
use Html\Element as E;
use Html\Fragment;
use Html\Slot;

// Anonymous + named slots routed to author-named parameters.
function Layout(
    string $lang,
    #[Slot]           Html\Htmlable $body,
    #[Slot('header')] ?Html\Htmlable $header = null,
    #[Slot('footer')] ?Html\Htmlable $footer = null,
): Html\Htmlable {
    return new E('main', ['lang' => $lang], [$header, $body, $footer]);
}

$out = render_component(
    'Layout',
    ['lang' => 'en'],
    new Fragment([new E('p', [], ['loose body'])]),          // anonymous slot
    [                                                          // named slots
        'header' => new E('h1', [], ['Title']),
        'footer' => new Fragment([new E('small', [], ['(c) 2026'])]),
    ],
);
echo $out, "\n";

// A component on a class constructor, anonymous slot only.
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
