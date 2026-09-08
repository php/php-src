--TEST--
DOM\Element Prototype.js like DOM-Traversal
--EXTENSIONS--
dom
--FILE--
<?php

$html = <<<HTML
<!DOCTYPE html>
<html>
<body>
    <div id="container">
        <span class="first" id="s1"></span>
        <span class="middle" id="s2"></span>
        <div class="middle" id="d1">
            <p id="p1"></p>
            <p id="p2"></p>
        </div>
        <span class="last" id="s3"></span>
    </div>
</body>
</html>
HTML;

$dom = DOM\HTMLDocument::createFromString($html);

$d1 = $dom->getElementById('d1');

// up()
var_dump($d1->up()->id); // container
var_dump($d1->up('body')->nodeName); // BODY
var_dump($d1->up('html', 0)->nodeName); // HTML

// down()
var_dump($d1->down()->id); // p1
var_dump($d1->down('p', 1)->id); // p2

// previous()
var_dump($d1->previous()->id); // s2
var_dump($d1->previous('span', 1)->id); // s1

// next()
var_dump($d1->next()->id); // s3
var_dump($d1->next('span')->id); // s3

// siblings()
$siblings = $d1->siblings();
echo count($siblings) . "\n";
foreach ($siblings as $sib) {
    echo $sib->id . "\n";
}

$spanSiblings = $d1->siblings('span');
echo count($spanSiblings) . "\n";
foreach ($spanSiblings as $sib) {
    echo $sib->id . "\n";
}

// Errors
try {
    $d1->up('div', -1);
} catch (ValueError $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
string(9) "container"
string(4) "BODY"
string(4) "HTML"
string(2) "p1"
string(2) "p2"
string(2) "s2"
string(2) "s1"
string(2) "s3"
string(2) "s3"
3
s1
s2
s3
3
s1
s2
s3
Dom\Element::up(): Argument #2 ($index) must be greater than or equal to 0
