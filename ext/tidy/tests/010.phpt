--TEST--
Accessing root, body, html, and head nodes..
--SKIPIF--
<?php if (!extension_loaded("tidy")) print "skip"; ?>
--FILE--
<?php 
    	$a = tidy_parse_string("<HTML><BODY BGCOLOR=#FFFFFF ALINK=#000000></BODY></HTML>");
        var_dump($a->root());
        var_dump($a->body());
        var_dump($a->html());
        var_dump($a->head());
 
?>
--EXPECT--
object(tidyNode)#2 (5) {
  ["value"]=>
  string(94) "<html>
<head>
<title></title>
</head>
<body bgcolor="#FFFFFF" alink="#000000">
</body>
</html>"
  ["name"]=>
  string(0) ""
  ["type"]=>
  int(0)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#3 (6) {
      ["value"]=>
      string(94) "<html>
<head>
<title></title>
</head>
<body bgcolor="#FFFFFF" alink="#000000">
</body>
</html>"
      ["name"]=>
      string(4) "html"
      ["type"]=>
      int(5)
      ["id"]=>
      int(48)
      ["attribute"]=>
      NULL
      ["child"]=>
      array(2) {
        [0]=>
        &object(tidyNode)#4 (6) {
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
            &object(tidyNode)#5 (6) {
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
        [1]=>
        &object(tidyNode)#6 (6) {
          ["value"]=>
          string(49) "<body bgcolor="#FFFFFF" alink="#000000">
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
          NULL
        }
      }
    }
  }
}
object(tidyNode)#2 (6) {
  ["value"]=>
  string(49) "<body bgcolor="#FFFFFF" alink="#000000">
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
  NULL
}
object(tidyNode)#2 (6) {
  ["value"]=>
  string(94) "<html>
<head>
<title></title>
</head>
<body bgcolor="#FFFFFF" alink="#000000">
</body>
</html>"
  ["name"]=>
  string(4) "html"
  ["type"]=>
  int(5)
  ["id"]=>
  int(48)
  ["attribute"]=>
  NULL
  ["child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#3 (6) {
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
        &object(tidyNode)#6 (6) {
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
    [1]=>
    &object(tidyNode)#4 (6) {
      ["value"]=>
      string(49) "<body bgcolor="#FFFFFF" alink="#000000">
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
      NULL
    }
  }
}
object(tidyNode)#2 (6) {
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
