--TEST--
Markup\Html: the injected default __toString() is callable under opcache with an observer active (class must skip the inheritance cache)
--EXTENSIONS--
markup
zend_test
--SKIPIF--
<?php if (!extension_loaded('Zend OPcache')) die('skip opcache not loaded'); ?>
--INI--
opcache.enable=1
opcache.enable_cli=1
zend_test.observer.enabled=1
zend_test.observer.show_output=0
--FILE--
<?php
declare(strict_types=1);

/* With opcache, a runtime-linked class goes through the inheritance cache:
 * the linked class is persisted to SHM and the SHM copy is used immediately.
 * zend_persist gives an arena-allocated internal method (like the injected
 * default __toString) a fresh per-request run_time_cache slot that nothing
 * initializes, and an observer's fcall-begin path dereferences it. Injecting
 * the method must therefore make the class non-cacheable, exactly like enums
 * ("We will add internal methods" in zend_do_link_class). The observer here
 * makes every function call walk that path. */

class Card implements Markup\Html {
    public function __construct(private string $title) {}
    public function toHtml(): Markup\Html {
        return <div class="card">{$this->title}</div>;
    }
}

class FancyCard extends Card {}

$c = new Card('a & b');
echo $c, "\n";                     // string cast -> zend_call_known_function
var_dump($c->__toString());       // direct VM call
echo new FancyCard('c'), "\n";     // inherited copy of the injected method
echo <Card title="tag" />, "\n";   // component dispatch path
?>
--EXPECT--
<div class="card">a &amp; b</div>
string(33) "<div class="card">a &amp; b</div>"
<div class="card">c</div>
<div class="card">tag</div>
