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
object(tidyNode)#3 (6) {
  ["value"]=>
  string(31) "<head>
<title></title>
</head>
"
  ["name"]=>
  string(4) "head"
  ["type"]=>
  int(5)
  ["id"]=>
  int(46)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#4 (6) {
      ["value"]=>
      string(16) "<title></title>
"
      ["name"]=>
      string(5) "title"
      ["type"]=>
      int(5)
      ["id"]=>
      int(111)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#4 (6) {
  ["value"]=>
  string(16) "<title></title>
"
  ["name"]=>
  string(5) "title"
  ["type"]=>
  int(5)
  ["id"]=>
  int(111)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
object(tidyNode)#5 (6) {
  ["value"]=>
  string(80) "<body bgcolor="#FFFFFF" alink="#000000">
<b>Hi</b><i>Bye<u>Test</u></i>
</body>
"
  ["name"]=>
  string(4) "body"
  ["type"]=>
  int(5)
  ["id"]=>
  int(16)
  ["attribute"]=>
  array(2) {
    ["bgcolor"]=>
    string(7) "#FFFFFF"
    ["alink"]=>
    string(7) "#000000"
  }
  ["child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#6 (6) {
      ["value"]=>
      string(9) "<b>Hi</b>"
      ["name"]=>
      string(1) "b"
      ["type"]=>
      int(5)
      ["id"]=>
      int(8)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(1) {
        [0]=>
        &object(tidyNode)#7 (5) {
          ["value"]=>
          string(2) "Hi"
          ["name"]=>
          string(0) ""
          ["type"]=>
          int(4)
          ["attribute"]=>
          NULL
          ["child"]=>
          NULL
        }
      }
    }
    [1]=>
    &object(tidyNode)#8 (6) {
      ["value"]=>
      string(21) "<i>Bye<u>Test</u></i>"
      ["name"]=>
      string(1) "i"
      ["type"]=>
      int(5)
      ["id"]=>
      int(49)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(2) {
        [0]=>
        &object(tidyNode)#9 (5) {
          ["value"]=>
          string(3) "Bye"
          ["name"]=>
          string(0) ""
          ["type"]=>
          int(4)
          ["attribute"]=>
          NULL
          ["child"]=>
          NULL
        }
        [1]=>
        &object(tidyNode)#10 (6) {
          ["value"]=>
          string(11) "<u>Test</u>"
          ["name"]=>
          string(1) "u"
          ["type"]=>
          int(5)
          ["id"]=>
          int(114)
          ["attribute"]=>
          NULL
          ["child"]=>
          array(1) {
            [0]=>
            &object(tidyNode)#11 (5) {
              ["value"]=>
              string(4) "Test"
              ["name"]=>
              string(0) ""
              ["type"]=>
              int(4)
              ["attribute"]=>
              NULL
              ["child"]=>
              NULL
            }
          }
        }
      }
    }
  }
}
bool(true)
object(tidyNode)#6 (6) {
  ["value"]=>
  string(9) "<b>Hi</b>"
  ["name"]=>
  string(1) "b"
  ["type"]=>
  int(5)
  ["id"]=>
  int(8)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#7 (5) {
      ["value"]=>
      string(2) "Hi"
      ["name"]=>
      string(0) ""
      ["type"]=>
      int(4)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#7 (5) {
  ["value"]=>
  string(2) "Hi"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(4)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
object(tidyNode)#8 (6) {
  ["value"]=>
  string(21) "<i>Bye<u>Test</u></i>"
  ["name"]=>
  string(1) "i"
  ["type"]=>
  int(5)
  ["id"]=>
  int(49)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#9 (5) {
      ["value"]=>
      string(3) "Bye"
      ["name"]=>
      string(0) ""
      ["type"]=>
      int(4)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
    [1]=>
    &object(tidyNode)#10 (6) {
      ["value"]=>
      string(11) "<u>Test</u>"
      ["name"]=>
      string(1) "u"
      ["type"]=>
      int(5)
      ["id"]=>
      int(114)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(1) {
        [0]=>
        &object(tidyNode)#11 (5) {
          ["value"]=>
          string(4) "Test"
          ["name"]=>
          string(0) ""
          ["type"]=>
          int(4)
          ["attribute"]=>
          NULL
          ["child"]=>
          NULL
        }
      }
    }
  }
}
bool(true)
object(tidyNode)#9 (5) {
  ["value"]=>
  string(3) "Bye"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(4)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
object(tidyNode)#10 (6) {
  ["value"]=>
  string(11) "<u>Test</u>"
  ["name"]=>
  string(1) "u"
  ["type"]=>
  int(5)
  ["id"]=>
  int(114)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#11 (5) {
      ["value"]=>
      string(4) "Test"
      ["name"]=>
      string(0) ""
      ["type"]=>
      int(4)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#11 (5) {
  ["value"]=>
  string(4) "Test"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(4)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
