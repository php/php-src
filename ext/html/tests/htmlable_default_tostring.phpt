--TEST--
Html\Htmlable: the default __toString() is injected at class-link time and yields to a declared one
--EXTENSIONS--
html
--FILE--
<?php
// A __toString declared by the class wins for string casts; markup rendering
// always goes through toHtml(), so the two can serve different audiences
// (e.g. a log-friendly string form) without affecting markup output.
class Custom implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return <b>markup</b>; }
    public function __toString(): string { return 'CUSTOM'; }
}
echo new Custom(), "\n";
echo <i>{new Custom()}</i>, "\n";

// A __toString flattened in from a trait counts as declared.
trait Loud {
    public function __toString(): string { return 'LOUD'; }
}
class UsesTrait implements Html\Htmlable {
    use Loud;
    public function toHtml(): Html\Htmlable { return Html\raw('quiet'); }
}
echo new UsesTrait(), " / ", <u>{new UsesTrait()}</u>, "\n";

// A __toString inherited from a parent class also wins.
class StringyBase {
    public function __toString(): string { return 'BASE'; }
}
class ChildOfStringy extends StringyBase implements Html\Htmlable {
    public function toHtml(): Html\Htmlable { return Html\raw('child'); }
}
echo new ChildOfStringy(), " / ", <u>{new ChildOfStringy()}</u>, "\n";

// An abstract class implementing Htmlable: concrete children inherit the
// injected default.
abstract class Widget implements Html\Htmlable {}
class Button extends Widget {
    public function toHtml(): Html\Htmlable { return <button>ok</button>; }
}
echo new Button(), "\n";

// An interface extending Htmlable stays abstract; implementers get the default.
interface Panel extends Html\Htmlable {}
class SidePanel implements Panel {
    public function toHtml(): Html\Htmlable { return <aside>side</aside>; }
}
echo new SidePanel(), "\n";

// The injected method is a real internal method: visible to reflection, with
// the declared string return type, owned by the implementing class.
$rm = new ReflectionMethod(SidePanel::class, '__toString');
var_dump($rm->isInternal(), (string) $rm->getReturnType(), $rm->getDeclaringClass()->getName());
var_dump(method_exists(new Button(), '__toString'));
var_dump(is_callable([new Button(), '__toString']));
echo (new Button())->__toString(), "\n";
?>
--EXPECT--
CUSTOM
<i><b>markup</b></i>
LOUD / <u>quiet</u>
BASE / <u>child</u>
<button>ok</button>
<aside>side</aside>
bool(true)
string(6) "string"
string(9) "SidePanel"
bool(true)
bool(true)
<button>ok</button>
