--TEST--
GH-14698 crash on DOM node dereference
--EXTENSIONS--
dom
simplexml
--CREDITS--
YuanchengJiang
--FILE--
<?php
  $dom = new DOMDocument;
  $dom->loadHTML('<span title="y">x</span><span title="z">x</span>');
  $html = simplexml_import_dom($dom);
  foreach ($html->body->span as $obj) {
  }
  $script1_dataflow = $html;
  $array = ['foo'];
  foreach ($array as $key => &$value) {
    unset($script1_dataflow[$key]);
  }
  echo "DONE";
?>
--EXPECTF--
DONE
