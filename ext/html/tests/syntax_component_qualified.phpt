--TEST--
Markup syntax: qualified/fully-qualified component tags and use resolution (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
namespace App {
    class Greeting implements \Html\Htmlable {
        public function __construct(#[\Html\Slot] public ?\Html\Htmlable $slot = null) {}
        public function toHtml(): \Html\Htmlable {
            return <p>{$this->slot}</p>;
        }
    }
    class Card implements \Html\Htmlable {
        public function __construct(public string $title) {}
        public function toHtml(): \Html\Htmlable {
            return new \Html\Element('b', [], [$this->title]);
        }
    }
    class Author {
        public static function byline(string $name): \Html\Htmlable {
            return new \Html\Element('i', [], ['By ', $name]);
        }
    }
}

namespace {
    // A qualified name is namespace-relative; from global scope App\Greeting is App\Greeting.
    echo (<App\Greeting>hi</App\Greeting>)->__toString(), "\n";
    echo (<App\Card title="G"/>)->__toString(), "\n";
}

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

namespace Wrong {
    // Without an import, a bare tag resolves in the current namespace only —
    // exactly how PHP resolves an unqualified class name (no global fallback).
    try {
        echo (<Card title="x"/>)->__toString(), "\n";
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
<p>hi</p>
<b>G</b>
<p>fq</p>
<b>C</b>
<i>By Ada</i>
<i>By Grace</i>
"Wrong\Card" is not a component: no such class implementing Html\Htmlable
