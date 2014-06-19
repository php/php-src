--TEST--
Bug #37587 (var without attribute causes segfault)
--SKIPIF--
<?php if (!extension_loaded("wddx")) print "skip"; ?>
--FILE--
<?php

var_dump(wddx_deserialize(<<<EOF
<wddxPacket version='1.0'>
<header/>
<data>
  <array length='1'>
    <var>
      <struct>
        <var name='test'><string>Hello World</string></var>
      </struct>
    </var>
  </array>
</data>
</wddxPacket>
EOF
));

?>
===DONE===
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    ["test"]=>
    string(11) "Hello World"
  }
}
===DONE===
