--TEST--
Re-constructing a DOMXPath does not expose freed php:function registrations to the cycle collector
--EXTENSIONS--
dom
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php
class GcElement extends DOMElement
{
    public function __destruct()
    {
        gc_collect_cycles();
    }
}

class Holder
{
    public $self;

    public function cb($node)
    {
        return true;
    }
}

$doc = new DOMDocument();
$doc->loadXML('<r><a/><b/><c/></r>');
$doc->registerNodeClass(DOMElement::class, GcElement::class);

$xp = new DOMXPath($doc);
$xp->registerNamespace('php', 'http://php.net/xpath');

$holder = new Holder();
$holder->self = $holder;
$xp->registerPhpFunctions(['cb' => [$holder, 'cb']]);

$xp->query('/r/*[php:function("cb", .)]');
unset($holder);

/* Make the object a collector root candidate, then re-construct it: the
   registration teardown must not stay reachable while it is being freed. */
$tmp = $xp;
unset($tmp);
$xp->__construct($doc);

var_dump($xp->query('/r/a')->length);
?>
--EXPECT--
int(1)
