--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - hierarchy variation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <alone/>
    <child><testelement/></child>
</container>
XML);

$container = $doc->documentElement;
$alone = $container->firstElementChild;
$testElement = $alone->nextElementSibling->firstElementChild;

try {
    $testElement->prepend($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->append($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->before($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->after($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->replaceWith($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $doc->saveXML();
?>
--EXPECT--
Hierarchy Request Error
Hierarchy Request Error
AddressSanitizer:DEADLYSIGNAL
=================================================================
==1135853==ERROR: AddressSanitizer: stack-overflow on address 0x7ffd657f8ff8 (pc 0x562d1f8639ed bp 0x7ffd657f9030 sp 0x7ffd657f8ff0 T0)
    #0 0x562d1f8639ed in dom_reconcile_ns_internal /home/niels/php-src/ext/dom/php_dom.c:1463
    #1 0x562d1f863ede in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1515
    #2 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #3 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #4 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #5 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #6 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #7 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #8 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #9 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #10 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #11 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #12 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #13 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #14 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #15 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #16 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #17 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #18 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #19 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #20 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #21 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #22 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #23 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #24 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #25 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #26 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #27 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #28 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #29 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #30 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #31 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #32 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #33 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #34 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #35 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #36 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #37 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #38 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #39 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #40 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #41 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #42 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #43 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #44 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #45 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #46 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #47 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #48 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #49 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #50 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #51 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #52 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #53 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #54 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #55 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #56 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #57 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #58 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #59 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #60 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #61 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #62 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #63 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #64 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #65 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #66 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #67 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #68 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #69 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #70 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #71 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #72 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #73 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #74 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #75 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #76 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #77 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #78 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #79 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #80 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #81 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #82 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #83 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #84 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #85 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #86 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #87 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #88 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #89 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #90 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #91 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #92 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #93 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #94 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #95 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #96 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #97 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #98 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #99 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #100 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #101 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #102 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #103 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #104 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #105 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #106 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #107 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #108 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #109 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #110 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #111 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #112 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #113 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #114 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #115 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #116 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #117 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #118 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #119 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #120 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #121 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #122 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #123 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #124 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #125 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #126 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #127 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #128 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #129 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #130 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #131 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #132 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #133 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #134 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #135 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #136 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #137 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #138 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #139 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #140 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #141 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #142 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #143 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #144 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #145 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #146 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #147 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #148 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #149 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #150 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #151 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #152 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #153 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #154 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #155 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #156 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #157 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #158 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #159 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #160 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #161 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #162 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #163 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #164 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #165 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #166 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #167 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #168 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #169 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #170 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #171 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #172 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #173 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #174 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #175 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #176 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #177 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #178 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #179 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #180 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #181 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #182 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #183 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #184 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #185 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #186 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #187 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #188 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #189 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #190 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #191 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #192 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #193 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #194 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #195 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #196 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #197 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #198 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #199 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #200 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #201 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #202 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #203 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #204 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #205 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #206 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #207 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #208 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #209 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #210 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #211 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #212 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #213 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #214 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #215 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #216 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #217 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #218 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #219 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #220 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #221 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #222 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #223 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #224 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #225 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #226 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #227 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #228 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #229 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #230 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #231 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #232 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #233 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #234 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #235 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #236 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #237 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #238 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #239 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #240 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #241 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #242 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #243 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #244 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #245 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517
    #246 0x562d1f863f55 in dom_reconcile_ns_list_internal /home/niels/php-src/ext/dom/php_dom.c:1517

SUMMARY: AddressSanitizer: stack-overflow /home/niels/php-src/ext/dom/php_dom.c:1463 in dom_reconcile_ns_internal
==1135853==ABORTING
