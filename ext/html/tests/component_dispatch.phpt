--TEST--
Html\render_component: class / function / static-method dispatch with props
--EXTENSIONS--
html
--FILE--
<?php
use function Html\render_component;
use Html\Element as E;

// (1) Class implementing Htmlable -> instantiated with named args.
class Card implements Html\Htmlable {
    public function __construct(public string $title, public string $tone = 'plain') {}
    public function toHtml(): Html\Htmlable {
        return new E('div', ['class' => "card card-{$this->tone}"], [$this->title]);
    }
}
echo render_component(Card::class, ['title' => 'Hi & bye', 'tone' => 'loud'])->__toString(), "\n";

// (2) Userland function returning Htmlable.
function Badge(string $label): Html\Htmlable {
    return new E('span', ['class' => 'badge'], [$label]);
}
echo render_component('Badge', ['label' => 'new']), "\n";

// (3) Static-method component.
class Author {
    public static function byline(string $name): Html\Htmlable {
        return new E('p', ['class' => 'byline'], ['By ', $name]);
    }
}
echo render_component('Author::byline', ['name' => 'Ada']), "\n";
?>
--EXPECT--
<div class="card card-loud">Hi &amp; bye</div>
<span class="badge">new</span>
<p class="byline">By Ada</p>
