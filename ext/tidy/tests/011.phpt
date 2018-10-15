--TEST--
Accessing attributes of a node
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php
    	$a = tidy_parse_string("<HTML><BODY BGCOLOR=#FFFFFF ALINK=#000000></BODY></HTML>");
        $body = $a->body();
        var_dump($body->attribute);
        foreach($body->attribute as $key=>$val) {
            echo "Attrib '$key': $val\n";
        }
?>
--EXPECT--
array(2) {
  ["bgcolor"]=>
  string(7) "#FFFFFF"
  ["alink"]=>
  string(7) "#000000"
}
Attrib 'bgcolor': #FFFFFF
Attrib 'alink': #000000
