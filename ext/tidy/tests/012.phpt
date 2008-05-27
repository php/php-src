--TEST--
Accessing children nodes
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php

        function dump_nodes(tidyNode $node) {

            var_dump($node->hasChildren());
            if($node->hasChildren()) {

                foreach($node->child as $c) {

                    var_dump($c);

                    if($c->hasChildren()) {

                        dump_nodes($c);

                    }
                }

            }

        }

    	$a = tidy_parse_string("<HTML><BODY BGCOLOR=#FFFFFF ALINK=#000000><B>Hi</B><I>Bye<U>Test</U></I></BODY></HTML>");
        $html = $a->html();
        dump_nodes($html);
            
?>
--EXPECT--
bool(true)
object(tidyNode)#3 (9) {
  [u"value"]=>
  unicode(31) "<head>
<title></title>
</head>
"
  [u"name"]=>
  unicode(4) "head"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(7)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(46)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#4 (9) {
      [u"value"]=>
      unicode(16) "<title></title>
"
      [u"name"]=>
      unicode(5) "title"
      [u"type"]=>
      int(5)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(87)
      [u"proprietary"]=>
      bool(false)
      [u"id"]=>
      int(111)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#4 (9) {
  [u"value"]=>
  unicode(16) "<title></title>
"
  [u"name"]=>
  unicode(5) "title"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(87)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(111)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  NULL
}
object(tidyNode)#5 (9) {
  [u"value"]=>
  unicode(80) "<body bgcolor="#FFFFFF" alink="#000000">
<b>Hi</b><i>Bye<u>Test</u></i>
</body>
"
  [u"name"]=>
  unicode(4) "body"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(7)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(16)
  [u"attribute"]=>
  array(2) {
    [u"bgcolor"]=>
    unicode(7) "#FFFFFF"
    [u"alink"]=>
    unicode(7) "#000000"
  }
  [u"child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#6 (9) {
      [u"value"]=>
      unicode(9) "<b>Hi</b>"
      [u"name"]=>
      unicode(1) "b"
      [u"type"]=>
      int(5)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(43)
      [u"proprietary"]=>
      bool(false)
      [u"id"]=>
      int(8)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      array(1) {
        [0]=>
        &object(tidyNode)#7 (8) {
          [u"value"]=>
          unicode(2) "Hi"
          [u"name"]=>
          unicode(0) ""
          [u"type"]=>
          int(4)
          [u"line"]=>
          int(1)
          [u"column"]=>
          int(46)
          [u"proprietary"]=>
          bool(false)
          [u"attribute"]=>
          NULL
          [u"child"]=>
          NULL
        }
      }
    }
    [1]=>
    &object(tidyNode)#8 (9) {
      [u"value"]=>
      unicode(21) "<i>Bye<u>Test</u></i>"
      [u"name"]=>
      unicode(1) "i"
      [u"type"]=>
      int(5)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(52)
      [u"proprietary"]=>
      bool(false)
      [u"id"]=>
      int(49)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      array(2) {
        [0]=>
        &object(tidyNode)#9 (8) {
          [u"value"]=>
          unicode(3) "Bye"
          [u"name"]=>
          unicode(0) ""
          [u"type"]=>
          int(4)
          [u"line"]=>
          int(1)
          [u"column"]=>
          int(55)
          [u"proprietary"]=>
          bool(false)
          [u"attribute"]=>
          NULL
          [u"child"]=>
          NULL
        }
        [1]=>
        &object(tidyNode)#10 (9) {
          [u"value"]=>
          unicode(11) "<u>Test</u>"
          [u"name"]=>
          unicode(1) "u"
          [u"type"]=>
          int(5)
          [u"line"]=>
          int(1)
          [u"column"]=>
          int(58)
          [u"proprietary"]=>
          bool(false)
          [u"id"]=>
          int(114)
          [u"attribute"]=>
          NULL
          [u"child"]=>
          array(1) {
            [0]=>
            &object(tidyNode)#11 (8) {
              [u"value"]=>
              unicode(4) "Test"
              [u"name"]=>
              unicode(0) ""
              [u"type"]=>
              int(4)
              [u"line"]=>
              int(1)
              [u"column"]=>
              int(61)
              [u"proprietary"]=>
              bool(false)
              [u"attribute"]=>
              NULL
              [u"child"]=>
              NULL
            }
          }
        }
      }
    }
  }
}
bool(true)
object(tidyNode)#6 (9) {
  [u"value"]=>
  unicode(9) "<b>Hi</b>"
  [u"name"]=>
  unicode(1) "b"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(43)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(8)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#7 (8) {
      [u"value"]=>
      unicode(2) "Hi"
      [u"name"]=>
      unicode(0) ""
      [u"type"]=>
      int(4)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(46)
      [u"proprietary"]=>
      bool(false)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#7 (8) {
  [u"value"]=>
  unicode(2) "Hi"
  [u"name"]=>
  unicode(0) ""
  [u"type"]=>
  int(4)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(46)
  [u"proprietary"]=>
  bool(false)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  NULL
}
object(tidyNode)#8 (9) {
  [u"value"]=>
  unicode(21) "<i>Bye<u>Test</u></i>"
  [u"name"]=>
  unicode(1) "i"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(52)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(49)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#9 (8) {
      [u"value"]=>
      unicode(3) "Bye"
      [u"name"]=>
      unicode(0) ""
      [u"type"]=>
      int(4)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(55)
      [u"proprietary"]=>
      bool(false)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      NULL
    }
    [1]=>
    &object(tidyNode)#10 (9) {
      [u"value"]=>
      unicode(11) "<u>Test</u>"
      [u"name"]=>
      unicode(1) "u"
      [u"type"]=>
      int(5)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(58)
      [u"proprietary"]=>
      bool(false)
      [u"id"]=>
      int(114)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      array(1) {
        [0]=>
        &object(tidyNode)#11 (8) {
          [u"value"]=>
          unicode(4) "Test"
          [u"name"]=>
          unicode(0) ""
          [u"type"]=>
          int(4)
          [u"line"]=>
          int(1)
          [u"column"]=>
          int(61)
          [u"proprietary"]=>
          bool(false)
          [u"attribute"]=>
          NULL
          [u"child"]=>
          NULL
        }
      }
    }
  }
}
bool(true)
object(tidyNode)#9 (8) {
  [u"value"]=>
  unicode(3) "Bye"
  [u"name"]=>
  unicode(0) ""
  [u"type"]=>
  int(4)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(55)
  [u"proprietary"]=>
  bool(false)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  NULL
}
object(tidyNode)#10 (9) {
  [u"value"]=>
  unicode(11) "<u>Test</u>"
  [u"name"]=>
  unicode(1) "u"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(58)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(114)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#11 (8) {
      [u"value"]=>
      unicode(4) "Test"
      [u"name"]=>
      unicode(0) ""
      [u"type"]=>
      int(4)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(61)
      [u"proprietary"]=>
      bool(false)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#11 (8) {
  [u"value"]=>
  unicode(4) "Test"
  [u"name"]=>
  unicode(0) ""
  [u"type"]=>
  int(4)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(61)
  [u"proprietary"]=>
  bool(false)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  NULL
}
