--TEST--
Bug #27010 (segfault and node text not displayed when returned from children())
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml=<<<EOF
<drinks xmlns:hot="http://www.example.com/hot">
 <hot:drink><hot:name>Coffee</hot:name></hot:drink>
 <hot:drink><hot:name>Tea</hot:name></hot:drink>
 <drink><name>Cola</name></drink>
 <drink><name>Juice</name></drink>
</drinks>
EOF;

$sxe = simplexml_load_string($xml);

foreach ($sxe as $element_name => $element) {
    print "$element_name is $element->name\n";
}

foreach ($sxe->children('http://www.example.com/hot') as $element_name => $element) {
    print "$element_name is $element->name\n";
}

?>
--EXPECT--
drink is Cola
drink is Juice
drink is Coffee
drink is Tea
