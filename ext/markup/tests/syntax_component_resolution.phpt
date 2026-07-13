--TEST--
Markup syntax: compile-time component-name resolution (use aliases, qualified/fully-qualified tags, static methods)
--EXTENSIONS--
markup
--FILE--
<?php
// --- component definitions ---
namespace App\Components {
    use Markup\Element as E;
    class Card implements \Markup\Html {
        public function __construct(public string $title) {}
        public function toHtml(): \Markup\Html {
            return new E('div', ['class' => 'card'], [$this->title]);
        }
    }
}

namespace App {
    class Greeting implements \Markup\Html {
        public function __construct(#[\Markup\Slot] public ?\Markup\Html $slot = null) {}
        public function toHtml(): \Markup\Html {
            return <p>{$this->slot}</p>;
        }
    }
    class Card implements \Markup\Html {
        public function __construct(public string $title) {}
        public function toHtml(): \Markup\Html {
            return new \Markup\Element('b', [], [$this->title]);
        }
    }
    class Author {
        public static function byline(string $name): \Markup\Html {
            return new \Markup\Element('i', [], ['By ', $name]);
        }
    }
}

// --- use-alias resolution from another namespace ---
namespace App\Views {
    use App\Components\Card;
    // <Card> resolves to App\Components\Card via the `use` alias, not a global Card.
    echo (<Card title="Hi & ok"/>)->__toString(), "\n";
    echo (<div><Card title="Nested"/></div>)->__toString(), "\n";
}

// --- qualified (namespace-relative) tags from global scope ---
namespace {
    // A qualified name is namespace-relative; from global scope App\Greeting is App\Greeting.
    echo (<App\Greeting>hi</App\Greeting>)->__toString(), "\n";
    echo (<App\Card title="G"/>)->__toString(), "\n";
}

// --- fully-qualified tags, use imports, static-method class-part resolution ---
namespace Views {
    // A fully-qualified <\...> tag ignores the current namespace.
    echo (<\App\Greeting>fq</\App\Greeting>)->__toString(), "\n";

    // A class component is imported with `use`, like any class.
    use App\Card;
    echo (<Card title="C"/>)->__toString(), "\n";

    // A static-method tag's class part resolves the same way: qualified,
    // fully-qualified, or through the class `use` table.
    echo (<\App\Author::byline name="Ada"/>)->__toString(), "\n";
    use App\Author;
    echo (<Author::byline name="Grace"/>)->__toString(), "\n";
}

// --- component defined and used in the current namespace ---
namespace Other {
    use Markup\Element as E;
    class Box implements \Markup\Html {
        public function __construct(public string $label) {}
        public function toHtml(): \Markup\Html { return new E('b', [], [$this->label]); }
    }
    echo (<Box label="local"/>)->__toString(), "\n";
}

// --- unimported bare tag fails: no global fallback ---
namespace Wrong {
    // Without an import, a bare tag resolves in the current namespace only -
    // exactly how PHP resolves an unqualified class name (no global fallback).
    try {
        echo (<Card title="x"/>)->__toString(), "\n";
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
<div class="card">Hi &amp; ok</div>
<div><div class="card">Nested</div></div>
<p>hi</p>
<b>G</b>
<p>fq</p>
<b>C</b>
<i>By Ada</i>
<i>By Grace</i>
<b>local</b>
"Wrong\Card" is not a component: no such class implementing Markup\Html
