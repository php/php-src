--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - type variation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <test/>
    <child><testelement/></child>
</container>
XML);

$testElement = $doc->documentElement->firstElementChild->nextElementSibling->firstElementChild;

try {
    $doc->documentElement->firstElementChild->prepend($testElement, 0);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->append($testElement, true);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->before($testElement, null);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->after($testElement, new stdClass);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->replaceWith($testElement, []);
} catch (\TypeError $e) {
    echo $e->getMessage(), "\n";
}

echo $doc->saveXML();
?>
--EXPECTF--
DOMElement::prepend(): Argument #2 must be of type DOMNode|string, int given
=================================================================
==1135855==ERROR: AddressSanitizer: heap-use-after-free on address 0x60c0000aef80 at pc 0x55a9f3e62cc1 bp 0x7fff33f0cde0 sp 0x7fff33f0cdd0
READ of size 8 at 0x60c0000aef80 thread T0
    #0 0x55a9f3e62cc0 in dom_hierarchy /home/niels/php-src/ext/dom/php_dom.c:1304
    #1 0x55a9f3e78218 in dom_hierarchy_node_list /home/niels/php-src/ext/dom/parentnode.c:274
    #2 0x55a9f3e78273 in dom_parent_node_append /home/niels/php-src/ext/dom/parentnode.c:289
    #3 0x55a9f3e88cee in zim_DOMElement_append /home/niels/php-src/ext/dom/element.c:1189
    #4 0x55a9f46151ec in ZEND_DO_FCALL_SPEC_RETVAL_UNUSED_HANDLER /home/niels/php-src/Zend/zend_vm_execute.h:1761
    #5 0x55a9f475b3bc in execute_ex /home/niels/php-src/Zend/zend_vm_execute.h:55819
    #6 0x55a9f476d276 in zend_execute /home/niels/php-src/Zend/zend_vm_execute.h:60163
    #7 0x55a9f457c57a in zend_execute_scripts /home/niels/php-src/Zend/zend.c:1846
    #8 0x55a9f4411a4a in php_execute_script /home/niels/php-src/main/main.c:2542
    #9 0x55a9f4946113 in do_cli /home/niels/php-src/sapi/cli/php_cli.c:965
    #10 0x55a9f4948391 in main %s:%d
    #11 0x7fc94cc9784f  (/usr/lib/libc.so.6+0x2384f) (BuildId: 2f005a79cd1a8e385972f5a102f16adba414d75e)
    #12 0x7fc94cc97909 in __libc_start_main (/usr/lib/libc.so.6+0x23909) (BuildId: 2f005a79cd1a8e385972f5a102f16adba414d75e)
    #13 0x55a9f3c03964 in _start (/home/niels/php-src/sapi/cli/php+0x403964) (BuildId: fe4174ba3c0d6fdc19db756f37e7f7f681145c4d)

0x60c0000aef80 is located 64 bytes inside of 120-byte region [0x60c0000aef40,0x60c0000aefb8)
freed by thread T0 here:
    #0 0x7fc94d2dfdc2 in __interceptor_free /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:52
    #1 0x7fc94cfb0029 in xmlFreeNodeList /home/niels/libxml2/tree.c:3817

previously allocated by thread T0 here:
    #0 0x7fc94d2e1369 in __interceptor_malloc /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:69
    #1 0x7fc94cfaebd0 in xmlNewNodeEatName /home/niels/libxml2/tree.c:2317

SUMMARY: AddressSanitizer: heap-use-after-free /home/niels/php-src/ext/dom/php_dom.c:1304 in dom_hierarchy
Shadow bytes around the buggy address:
  0x60c0000aed00: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 fa
  0x60c0000aed80: fa fa fa fa fa fa fa fa 00 00 00 00 00 00 00 00
  0x60c0000aee00: 00 00 00 00 00 00 00 fa fa fa fa fa fa fa fa fa
  0x60c0000aee80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 fa
  0x60c0000aef00: fa fa fa fa fa fa fa fa fd fd fd fd fd fd fd fd
=>0x60c0000aef80:[fd]fd fd fd fd fd fd fa fa fa fa fa fa fa fa fa
  0x60c0000af000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 fa
  0x60c0000af080: fa fa fa fa fa fa fa fa fd fd fd fd fd fd fd fd
  0x60c0000af100: fd fd fd fd fd fd fd fa fa fa fa fa fa fa fa fa
  0x60c0000af180: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x60c0000af200: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==1135855==ABORTING
