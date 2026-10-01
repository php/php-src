--TEST--
Integer offsets that cannot resolve must never alias or mutate a node
--EXTENSIONS--
simplexml
--FILE--
<?php
function fresh(): SimpleXMLElement {
    return simplexml_load_string('<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>');
}

function state(SimpleXMLElement $x): string {
    return trim(strstr($x->asXML(), '<r'));
}

echo "== element list ==\n";
$x = fresh();
var_dump(isset($x->item[-1]));
var_dump($x->item[-1]);
var_dump((string) $x->item[0]);
$x->item[-1] = 'Z';
echo state($x), "\n";
unset($x->item[-1]);
echo state($x), "\n";
$x->item[5] = 'P';
echo state($x), "\n";

echo "== single element ==\n";
$x = fresh();
$n = $x->item[0];
var_dump(isset($n[-1]));
var_dump($n[-1]);
var_dump($n[5]);
$n[-1] = 'Z';
echo state($x), "\n";
$n[5] = 'Y';
echo state($x), "\n";
unset($n[-1]);
echo state($x), "\n";
var_dump((string) $n[0]);

echo "== nested write ==\n";
$x = fresh();
try {
    $x->item[-1]->kid = 'K';
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
echo state($x), "\n";

echo "== attributes ==\n";
$x = fresh();
$at = $x->attributes();
var_dump(isset($at[-1]));
var_dump($at[-1]);
$at[-1] = 'z';
echo state($x), "\n";
?>
--EXPECTF--
== element list ==
bool(false)
NULL
string(1) "a"

Warning: main(): Cannot add element item number -1 when only 3 such elements exist in %s on line %d
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>

Warning: main(): Cannot add element item number 5 when only 3 such elements exist in %s on line %d
<r a="1" b="2"><item>a</item><item>b</item><item>c</item><item>P</item></r>
== single element ==
bool(false)
NULL
NULL

Warning: main(): Cannot add element item number -1 when only 0 such elements exist in %s on line %d
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>

Warning: main(): Cannot add element item number 5 when only 0 such elements exist in %s on line %d
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>
string(1) "a"
== nested write ==

Warning: main(): Cannot add element item number -1 when only 3 such elements exist in %s on line %d

Notice: Indirect modification of overloaded element of SimpleXMLElement has no effect in %s on line %d
Error: Attempt to assign property "kid" on null
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>
== attributes ==
bool(false)
NULL

Warning: main(): Cannot change attribute number -1 when only 0 attributes exist in %s on line %d
<r a="1" b="2"><item>a</item><item>b</item><item>c</item></r>
