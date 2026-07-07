--TEST--
Html\Htmlable: the injected default __toString() survives opcache (arena-allocated internal method)
--EXTENSIONS--
html
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die('skip opcache not loaded'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
--FILE--
<?php
declare(strict_types=1);

class Card implements Html\Htmlable {
    public function __construct(private string $title) {}
    public function toHtml(): Html\Htmlable {
        return <div class="card">{$this->title}</div>;
    }
}

// Inheritance pushes the linked class (with its injected internal method)
// through opcache's inheritance cache / persistence paths.
class FancyCard extends Card {}

echo new Card('a & b'), "\n";
echo new FancyCard('c'), "\n";
echo <section>{new Card('nested')}</section>, "\n";
var_dump((new ReflectionMethod(FancyCard::class, '__toString'))->isInternal());
?>
--EXPECT--
<div class="card">a &amp; b</div>
<div class="card">c</div>
<section><div class="card">nested</div></section>
bool(true)
