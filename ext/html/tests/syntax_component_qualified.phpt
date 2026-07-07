--TEST--
Markup syntax: qualified/fully-qualified component tags and use-function resolution (RFC §4)
--EXTENSIONS--
html
--FILE--
<?php
namespace App {
    // A function component and a class component sharing the App namespace.
    function Greeting(#[\Html\Slot] $slot): \Html\Htmlable {
        return <p>{$slot}</p>;
    }
    class Card implements \Html\Htmlable {
        public function __construct(public string $title) {}
        public function toHtml(): \Html\Htmlable {
            return new \Html\Element('b', [], [$this->title]);
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

    // A function component is imported with `use function` (not `use`).
    use function App\Greeting;
    echo (<Greeting>uf</Greeting>)->__toString(), "\n";

    // A class component is imported with a class `use`.
    use App\Card;
    echo (<Card title="C"/>)->__toString(), "\n";
}

namespace Wrong {
    // A class `use` does NOT bring a *function* component into scope: markup
    // resolves the class candidate (App\Greeting, not a class) and the function
    // candidate (Wrong\Greeting, no such function) and rejects both, exactly as
    // ordinary PHP keeps class and function name resolution separate.
    use App\Greeting;
    try {
        echo (<Greeting>x</Greeting>)->__toString(), "\n";
    } catch (\Error $e) {
        echo $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
<p>hi</p>
<b>G</b>
<p>fq</p>
<p>uf</p>
<b>C</b>
"App\Greeting" is not a component: no class implementing Html\Htmlable and no user-defined function of that name
