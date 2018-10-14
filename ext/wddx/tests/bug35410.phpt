--TEST--
#35410 (wddx_deserialize() doesn't handle large ints as keys properly)
--SKIPIF--
<?php
	if (!extension_loaded("wddx")) print "skip";
    if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
$wddx = <<<WDX
<wddxpacket version="1.0">
<header>
<comment>Content Configuration File</comment>
</header>
<data>
<struct>
<var name="content_queries">
<struct>
<var name="content_113300831086270200">
<struct>
<var name="113301888545229100">
<struct>
<var name="max">
<number>10</number>
</var>
<var name="cache">
<number>4</number>
</var>
<var name="order">
<struct>
<var name="content_113300831086270200">
<struct>
<var name="CMS_BUILD">
<string>desc</string>
</var>
</struct>
</var>
</struct>
</var>
</struct>
</var>
</struct>
</var>
</struct>
</var>
</struct>
</data>
</wddxpacket>
WDX;

var_dump(wddx_deserialize($wddx));
?>
--EXPECT--
array(1) {
  ["content_queries"]=>
  array(1) {
    ["content_113300831086270200"]=>
    array(1) {
      ["113301888545229100"]=>
      array(3) {
        ["max"]=>
        int(10)
        ["cache"]=>
        int(4)
        ["order"]=>
        array(1) {
          ["content_113300831086270200"]=>
          array(1) {
            ["CMS_BUILD"]=>
            string(4) "desc"
          }
        }
      }
    }
  }
}
