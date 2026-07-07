--TEST--
Markup syntax: component names are resolved at compile time honoring use/namespace (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
namespace App\Components {
    use Html\Element as E;
    class Card implements \Html\Htmlable {
        public function __construct(public string $title) {}
        public function toHtml(): \Html\Htmlable {
            return new E('div', ['class' => 'card'], [$this->title]);
        }
    }
}

namespace App\Views {
    use App\Components\Card;
    // <Card> resolves to App\Components\Card via the `use` alias, not a global Card.
    echo (<Card title="Hi & ok"/>)->__toString(), "\n";
    echo (<div><Card title="Nested"/></div>)->__toString(), "\n";
}

namespace Other {
    // A fully-qualified-feeling name via current namespace: define a local component.
    use Html\Element as E;
    class Box implements \Html\Htmlable {
        public function __construct(public string $label) {}
        public function toHtml(): \Html\Htmlable { return new E('b', [], [$this->label]); }
    }
    echo (<Box label="local"/>)->__toString(), "\n";
}
?>
--EXPECT--
<div class="card">Hi &amp; ok</div>
<div><div class="card">Nested</div></div>
<b>local</b>
