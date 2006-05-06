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
  ["value"]=>
  string(31) "<head>
<title></title>
</head>
"
  ["name"]=>
  string(4) "head"
  ["type"]=>
  int(5)
  ["line"]=>
  int(1)
  ["column"]=>
  int(7)
  ["proprietary"]=>
  bool(false)
  ["id"]=>
  int(46)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#4 (9) {
      ["value"]=>
      string(16) "<title></title>
"
      ["name"]=>
      string(5) "title"
      ["type"]=>
      int(5)
      ["line"]=>
      int(1)
      ["column"]=>
      int(87)
      ["proprietary"]=>
      bool(false)
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
object(tidyNode)#4 (9) {
  ["value"]=>
  string(16) "<title></title>
"
  ["name"]=>
  string(5) "title"
  ["type"]=>
  int(5)
  ["line"]=>
  int(1)
  ["column"]=>
  int(87)
  ["proprietary"]=>
  bool(false)
  ["id"]=>
  int(111)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
object(tidyNode)#5 (9) {
  ["value"]=>
  string(80) "<body bgcolor="#FFFFFF" alink="#000000">
<b>Hi</b><i>Bye<u>Test</u></i>
</body>
"
  ["name"]=>
  string(4) "body"
  ["type"]=>
  int(5)
  ["line"]=>
  int(1)
  ["column"]=>
  int(7)
  ["proprietary"]=>
  bool(false)
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
    &object(tidyNode)#6 (9) {
      ["value"]=>
      string(9) "<b>Hi</b>"
      ["name"]=>
      string(1) "b"
      ["type"]=>
      int(5)
      ["line"]=>
      int(1)
      ["column"]=>
      int(43)
      ["proprietary"]=>
      bool(false)
      ["id"]=>
      int(8)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(1) {
        [0]=>
        &object(tidyNode)#7 (8) {
          ["value"]=>
          string(2) "Hi"
          ["name"]=>
          string(0) ""
          ["type"]=>
          int(4)
          ["line"]=>
          int(1)
          ["column"]=>
          int(46)
          ["proprietary"]=>
          bool(false)
          ["attribute"]=>
          NULL
          ["child"]=>
          NULL
        }
      }
    }
    [1]=>
    &object(tidyNode)#8 (9) {
      ["value"]=>
      string(21) "<i>Bye<u>Test</u></i>"
      ["name"]=>
      string(1) "i"
      ["type"]=>
      int(5)
      ["line"]=>
      int(1)
      ["column"]=>
      int(52)
      ["proprietary"]=>
      bool(false)
      ["id"]=>
      int(49)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(2) {
        [0]=>
        &object(tidyNode)#9 (8) {
          ["value"]=>
          string(3) "Bye"
          ["name"]=>
          string(0) ""
          ["type"]=>
          int(4)
          ["line"]=>
          int(1)
          ["column"]=>
          int(55)
          ["proprietary"]=>
          bool(false)
          ["attribute"]=>
          NULL
          ["child"]=>
          NULL
        }
        [1]=>
        &object(tidyNode)#10 (9) {
          ["value"]=>
          string(11) "<u>Test</u>"
          ["name"]=>
          string(1) "u"
          ["type"]=>
          int(5)
          ["line"]=>
          int(1)
          ["column"]=>
          int(58)
          ["proprietary"]=>
          bool(false)
          ["id"]=>
          int(114)
          ["attribute"]=>
          NULL
          ["child"]=>
          array(1) {
            [0]=>
            &object(tidyNode)#11 (8) {
              ["value"]=>
              string(4) "Test"
              ["name"]=>
              string(0) ""
              ["type"]=>
              int(4)
              ["line"]=>
              int(1)
              ["column"]=>
              int(61)
              ["proprietary"]=>
              bool(false)
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
object(tidyNode)#6 (9) {
  ["value"]=>
  string(9) "<b>Hi</b>"
  ["name"]=>
  string(1) "b"
  ["type"]=>
  int(5)
  ["line"]=>
  int(1)
  ["column"]=>
  int(43)
  ["proprietary"]=>
  bool(false)
  ["id"]=>
  int(8)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#7 (8) {
      ["value"]=>
      string(2) "Hi"
      ["name"]=>
      string(0) ""
      ["type"]=>
      int(4)
      ["line"]=>
      int(1)
      ["column"]=>
      int(46)
      ["proprietary"]=>
      bool(false)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#7 (8) {
  ["value"]=>
  string(2) "Hi"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(4)
  ["line"]=>
  int(1)
  ["column"]=>
  int(46)
  ["proprietary"]=>
  bool(false)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
object(tidyNode)#8 (9) {
  ["value"]=>
  string(21) "<i>Bye<u>Test</u></i>"
  ["name"]=>
  string(1) "i"
  ["type"]=>
  int(5)
  ["line"]=>
  int(1)
  ["column"]=>
  int(52)
  ["proprietary"]=>
  bool(false)
  ["id"]=>
  int(49)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#9 (8) {
      ["value"]=>
      string(3) "Bye"
      ["name"]=>
      string(0) ""
      ["type"]=>
      int(4)
      ["line"]=>
      int(1)
      ["column"]=>
      int(55)
      ["proprietary"]=>
      bool(false)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
    [1]=>
    &object(tidyNode)#10 (9) {
      ["value"]=>
      string(11) "<u>Test</u>"
      ["name"]=>
      string(1) "u"
      ["type"]=>
      int(5)
      ["line"]=>
      int(1)
      ["column"]=>
      int(58)
      ["proprietary"]=>
      bool(false)
      ["id"]=>
      int(114)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(1) {
        [0]=>
        &object(tidyNode)#11 (8) {
          ["value"]=>
          string(4) "Test"
          ["name"]=>
          string(0) ""
          ["type"]=>
          int(4)
          ["line"]=>
          int(1)
          ["column"]=>
          int(61)
          ["proprietary"]=>
          bool(false)
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
object(tidyNode)#9 (8) {
  ["value"]=>
  string(3) "Bye"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(4)
  ["line"]=>
  int(1)
  ["column"]=>
  int(55)
  ["proprietary"]=>
  bool(false)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
object(tidyNode)#10 (9) {
  ["value"]=>
  string(11) "<u>Test</u>"
  ["name"]=>
  string(1) "u"
  ["type"]=>
  int(5)
  ["line"]=>
  int(1)
  ["column"]=>
  int(58)
  ["proprietary"]=>
  bool(false)
  ["id"]=>
  int(114)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#11 (8) {
      ["value"]=>
      string(4) "Test"
      ["name"]=>
      string(0) ""
      ["type"]=>
      int(4)
      ["line"]=>
      int(1)
      ["column"]=>
      int(61)
      ["proprietary"]=>
      bool(false)
      ["attribute"]=>
      NULL
      ["child"]=>
      NULL
    }
  }
}
bool(true)
object(tidyNode)#11 (8) {
  ["value"]=>
  string(4) "Test"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(4)
  ["line"]=>
  int(1)
  ["column"]=>
  int(61)
  ["proprietary"]=>
  bool(false)
  ["attribute"]=>
  NULL
  ["child"]=>
  NULL
}
--UEXPECT--
bool(true)
object(tidyNode)#3 (9) {
  [u"value"]=>
  string(31) "<head>
<title></title>
</head>
"
  [u"name"]=>
  string(4) "head"
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
      string(16) "<title></title>
"
      [u"name"]=>
      string(5) "title"
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
  string(16) "<title></title>
"
  [u"name"]=>
  string(5) "title"
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
  string(80) "<body bgcolor="#FFFFFF" alink="#000000">
<b>Hi</b><i>Bye<u>Test</u></i>
</body>
"
  [u"name"]=>
  string(4) "body"
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
    string(7) "#FFFFFF"
    [u"alink"]=>
    string(7) "#000000"
  }
  [u"child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#6 (9) {
      [u"value"]=>
      string(9) "<b>Hi</b>"
      [u"name"]=>
      string(1) "b"
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
          string(2) "Hi"
          [u"name"]=>
          string(0) ""
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
      string(21) "<i>Bye<u>Test</u></i>"
      [u"name"]=>
      string(1) "i"
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
          string(3) "Bye"
          [u"name"]=>
          string(0) ""
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
          string(11) "<u>Test</u>"
          [u"name"]=>
          string(1) "u"
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
              string(4) "Test"
              [u"name"]=>
              string(0) ""
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
  string(9) "<b>Hi</b>"
  [u"name"]=>
  string(1) "b"
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
      string(2) "Hi"
      [u"name"]=>
      string(0) ""
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
  string(2) "Hi"
  [u"name"]=>
  string(0) ""
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
  string(21) "<i>Bye<u>Test</u></i>"
  [u"name"]=>
  string(1) "i"
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
      string(3) "Bye"
      [u"name"]=>
      string(0) ""
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
      string(11) "<u>Test</u>"
      [u"name"]=>
      string(1) "u"
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
          string(4) "Test"
          [u"name"]=>
          string(0) ""
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
  string(3) "Bye"
  [u"name"]=>
  string(0) ""
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
  string(11) "<u>Test</u>"
  [u"name"]=>
  string(1) "u"
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
      string(4) "Test"
      [u"name"]=>
      string(0) ""
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
  string(4) "Test"
  [u"name"]=>
  string(0) ""
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
