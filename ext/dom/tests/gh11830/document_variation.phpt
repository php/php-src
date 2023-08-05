--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - document variation
--EXTENSIONS--
dom
--FILE--
<?php
$otherDoc = new DOMDocument;
$otherDoc->loadXML(<<<XML
<?xml version="1.0"?>
<other/>
XML);

$otherElement = $otherDoc->documentElement;

$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <alone/>
    <child><testelement/></child>
</container>
XML);

$testElement = $doc->documentElement->firstElementChild->nextElementSibling->firstElementChild;

try {
    $doc->documentElement->firstElementChild->prepend($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->append($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->before($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->after($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->replaceWith($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $otherDoc->saveXML();
echo $doc->saveXML();
?>
--EXPECTF--
Wrong Document Error
=================================================================
==1135851==ERROR: AddressSanitizer: heap-use-after-free on address 0x60c0000af1c0 at pc 0x565380462cc1 bp 0x7ffed07874f0 sp 0x7ffed07874e0
READ of size 8 at 0x60c0000af1c0 thread T0
    #0 0x565380462cc0 in dom_hierarchy /home/niels/php-src/ext/dom/php_dom.c:1304
    #1 0x565380478218 in dom_hierarchy_node_list /home/niels/php-src/ext/dom/parentnode.c:274
    #2 0x565380478273 in dom_parent_node_append /home/niels/php-src/ext/dom/parentnode.c:289
    #3 0x565380488cee in zim_DOMElement_append /home/niels/php-src/ext/dom/element.c:1189
    #4 0x565380c151ec in ZEND_DO_FCALL_SPEC_RETVAL_UNUSED_HANDLER /home/niels/php-src/Zend/zend_vm_execute.h:1761
    #5 0x565380d5b3bc in execute_ex /home/niels/php-src/Zend/zend_vm_execute.h:55819
    #6 0x565380d6d276 in zend_execute /home/niels/php-src/Zend/zend_vm_execute.h:60163
    #7 0x565380b7c57a in zend_execute_scripts /home/niels/php-src/Zend/zend.c:1846
    #8 0x565380a11a4a in php_execute_script /home/niels/php-src/main/main.c:2542
    #9 0x565380f46113 in do_cli /home/niels/php-src/sapi/cli/php_cli.c:965
    #10 0x565380f48391 in main %s:%d
    #11 0x7fbda2d9e84f  (/usr/lib/libc.so.6+0x2384f) (BuildId: 2f005a79cd1a8e385972f5a102f16adba414d75e)
    #12 0x7fbda2d9e909 in __libc_start_main (/usr/lib/libc.so.6+0x23909) (BuildId: 2f005a79cd1a8e385972f5a102f16adba414d75e)
    #13 0x565380203964 in _start (/home/niels/php-src/sapi/cli/php+0x403964) (BuildId: fe4174ba3c0d6fdc19db756f37e7f7f681145c4d)

0x60c0000af1c0 is located 64 bytes inside of 120-byte region [0x60c0000af180,0x60c0000af1f8)
freed by thread T0 here:
    #0 0x7fbda32dfdc2 in __interceptor_free /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:52
    #1 0x7fbda3104029 in xmlFreeNodeList /home/niels/libxml2/tree.c:3817

previously allocated by thread T0 here:
    #0 0x7fbda32e1369 in __interceptor_malloc /usr/src/debug/gcc/gcc/libsanitizer/asan/asan_malloc_linux.cpp:69
    #1 0x7fbda3102bd0 in xmlNewNodeEatName /home/niels/libxml2/tree.c:2317

SUMMARY: AddressSanitizer: heap-use-after-free /home/niels/php-src/ext/dom/php_dom.c:1304 in dom_hierarchy
Shadow bytes around the buggy address:
  0x60c0000aef00: fa fa fa fa fa fa fa fa 00 00 00 00 00 00 00 00
  0x60c0000aef80: 00 00 00 00 00 00 00 fa fa fa fa fa fa fa fa fa
  0x60c0000af000: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 fa
  0x60c0000af080: fa fa fa fa fa fa fa fa 00 00 00 00 00 00 00 00
  0x60c0000af100: 00 00 00 00 00 00 00 fa fa fa fa fa fa fa fa fa
=>0x60c0000af180: fd fd fd fd fd fd fd fd[fd]fd fd fd fd fd fd fa
  0x60c0000af200: fa fa fa fa fa fa fa fa 00 00 00 00 00 00 00 00
  0x60c0000af280: 00 00 00 00 00 00 00 fa fa fa fa fa fa fa fa fa
  0x60c0000af300: fd fd fd fd fd fd fd fd fd fd fd fd fd fd fd fa
  0x60c0000af380: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x60c0000af400: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
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
==1135851==ABORTING
