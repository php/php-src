--TEST--
Markup syntax: component tags lower to Html\render_component (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
use Html\Element as E;
use Html\Slot;

// Class component implementing Htmlable, with a prop and an anonymous slot.
class Card implements Html\Htmlable {
    public function __construct(
        public string $title,
        #[Slot] public ?Html\Htmlable $body = null,
    ) {}
    public function toHtml(): Html\Htmlable {
        return new E('section', ['class' => 'card'], [new E('h2', [], [$this->title]), $this->body]);
    }
}

// A second, minimal component for nesting cases.
class Badge implements Html\Htmlable {
    public function __construct(public string $label) {}
    public function toHtml(): Html\Htmlable {
        return new E('span', ['class' => 'badge'], [$this->label]);
    }
}

// Static-method component - multiple components can live together on one class.
class Author {
    public static function byline(string $name): Html\Htmlable {
        return new E('p', ['class' => 'by'], ['By ', $name]);
    }
}

$who = 'Ada & co';

// class component: attribute prop + body routed to the anonymous slot
echo (<Card title={$who}><p>Hi {$who}</p></Card>)->__toString(), "\n";

// self-closing class component (no body -> slot is null)
echo (<Card title="Solo"/>)->__toString(), "\n";

// static-method component
echo (<Author::byline name={$who}/>)->__toString(), "\n";

// the same dispatch, called directly by name
echo Html\render_component(Badge::class, ['label' => 'New &'])->__toString(), "\n";

// a component used as a child of an HTML element, and inside interpolation
echo (<div class="wrap"><Badge label="x"/></div>)->__toString(), "\n";
echo (<ul>{array_map(fn($t) => <Badge label={$t}/>, ['a', 'b'])}</ul>)->__toString(), "\n";
?>
--EXPECT--
<section class="card"><h2>Ada &amp; co</h2><p>Hi Ada &amp; co</p></section>
<section class="card"><h2>Solo</h2></section>
<p class="by">By Ada &amp; co</p>
<span class="badge">New &amp;</span>
<div class="wrap"><span class="badge">x</span></div>
<ul><span class="badge">a</span><span class="badge">b</span></ul>
