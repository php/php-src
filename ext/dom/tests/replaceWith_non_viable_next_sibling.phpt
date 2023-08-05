--TEST--
replaceWith() with a non-variable next sibling
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <child>
        <alone/>
    </child>
</container>
XML);

$container = $doc->documentElement;
$child = $container->firstElementChild;
$alone = $child->firstElementChild;

$child->after($alone);
echo $doc->saveXML();
$child->replaceWith($alone);
echo $doc->saveXML();
?>
--EXPECT--
<?xml version="1.0"?>
<container>
    <child>
        
    </child><alone/>
</container>
<?xml version="1.0"?>
<container>
    
</container>

=================================================================
==1151863==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 120 byte(s) in 1 object(s) allocated from:
    #0 0x7fc122ee1369 in __interceptor_malloc /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:69
    #1 0x7fc122d02bd0 in xmlNewNodeEatName /home/niels/libxml2/tree.c:2317

SUMMARY: AddressSanitizer: 120 byte(s) leaked in 1 allocation(s).
