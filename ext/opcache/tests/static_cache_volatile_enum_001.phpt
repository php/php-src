--TEST--
OPcache volatile cache: enum cases round-trip on the fast path as the process-global singleton (not instantiated), top-level, nested, and across a shared graph
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.static_cache.volatile_size_mb=32
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
use OPcache\VolatileCache as VC;

enum K_Suit: string
{
    case Hearts = 'H';
    case Spades = 'S';
}

enum K_Plain
{
    case A;
    case B;
}

class K_Card
{
    public function __construct(public K_Suit $suit) {}
}

class K_Node
{
    public ?object $a = null;
    public ?object $b = null;
    public ?K_Suit $suit = null;
}

/* A backed enum case stored directly: fast path, restored as the singleton. */
VC::getInstance('default')->store('ek_backed', K_Suit::Hearts);
$g = VC::getInstance('default')->fetch('ek_backed');
echo "backed is singleton: ";
var_dump($g === K_Suit::Hearts);
echo "backed value: ", $g->value, "\n";

/* A pure enum case. */
VC::getInstance('default')->store('ek_plain', K_Plain::B);
echo "plain is singleton: ";
var_dump(VC::getInstance('default')->fetch('ek_plain') === K_Plain::B);

/* Enum cases nested in an array. */
VC::getInstance('default')->store('ek_arr', ['suit' => K_Suit::Spades, 'plain' => K_Plain::A, 'n' => 5]);
$ga = VC::getInstance('default')->fetch('ek_arr');
echo "array enums: ";
var_dump($ga['suit'] === K_Suit::Spades && $ga['plain'] === K_Plain::A && $ga['n'] === 5);

/* Enum case as an object property. */
VC::getInstance('default')->store('ek_card', new K_Card(K_Suit::Spades));
echo "card enum: ";
var_dump(VC::getInstance('default')->fetch('ek_card')->suit === K_Suit::Spades);

/* Enum inside a shared-identity graph (prototype kept, cloned per fetch): the
 * shared identity and the enum singleton both survive, and fetches are distinct. */
$shared = new K_Node();
$shared->suit = K_Suit::Hearts;
$root = new K_Node();
$root->a = $shared;
$root->b = $shared;
VC::getInstance('default')->store('ek_graph', $root);
$x = VC::getInstance('default')->fetch('ek_graph');
$y = VC::getInstance('default')->fetch('ek_graph');
echo "graph identity + singleton: ";
var_dump($x->a === $x->b && $x->a->suit === K_Suit::Hearts);
echo "graph fetches distinct, both singleton: ";
var_dump($x->a !== $y->a && $y->a->suit === K_Suit::Hearts);
?>
--EXPECT--
backed is singleton: bool(true)
backed value: H
plain is singleton: bool(true)
array enums: bool(true)
card enum: bool(true)
graph identity + singleton: bool(true)
graph fetches distinct, both singleton: bool(true)
