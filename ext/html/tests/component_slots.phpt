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
class Layout implements Html\Htmlable {
    public function __construct(
        public string $lang,
        #[Slot] public ?Html\Htmlable $body = null,
        public ?Html\Htmlable $header = null,
        public ?Html\Htmlable $footer = null,
    ) {}
    public function toHtml(): Html\Htmlable {
        return new E('main', ['lang' => $this->lang], [$this->header, $this->body, $this->footer]);
    }
}

$out = render_component(
    Layout::class,
    [
        'lang'   => 'en',
        'header' => new E('h1', [], ['Title']),
        'footer' => new Fragment([new E('small', [], ['(c) 2026'])]),
    ],
    new Fragment([new E('p', [], ['loose body'])]),          // body slot
);
echo $out, "\n";

// Body slot only.
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
