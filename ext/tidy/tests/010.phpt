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
object(tidyNode)#2 (8) {
  [u"value"]=>
  unicode(94) "<html>
<head>
<title></title>
</head>
<body bgcolor="#FFFFFF" alink="#000000">
</body>
</html>"
  [u"name"]=>
  unicode(0) ""
  [u"type"]=>
  int(0)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(1)
  [u"proprietary"]=>
  bool(false)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  array(1) {
    [0]=>
    &object(tidyNode)#3 (9) {
      [u"value"]=>
      unicode(94) "<html>
<head>
<title></title>
</head>
<body bgcolor="#FFFFFF" alink="#000000">
</body>
</html>"
      [u"name"]=>
      unicode(4) "html"
      [u"type"]=>
      int(5)
      [u"line"]=>
      int(1)
      [u"column"]=>
      int(1)
      [u"proprietary"]=>
      bool(false)
      [u"id"]=>
      int(48)
      [u"attribute"]=>
      NULL
      [u"child"]=>
      array(2) {
        [0]=>
        &object(tidyNode)#4 (9) {
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
            &object(tidyNode)#5 (9) {
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
              int(57)
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
        [1]=>
        &object(tidyNode)#6 (9) {
          [u"value"]=>
          unicode(49) "<body bgcolor="#FFFFFF" alink="#000000">
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
          NULL
        }
      }
    }
  }
}
object(tidyNode)#2 (9) {
  [u"value"]=>
  unicode(49) "<body bgcolor="#FFFFFF" alink="#000000">
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
  NULL
}
object(tidyNode)#2 (9) {
  [u"value"]=>
  unicode(94) "<html>
<head>
<title></title>
</head>
<body bgcolor="#FFFFFF" alink="#000000">
</body>
</html>"
  [u"name"]=>
  unicode(4) "html"
  [u"type"]=>
  int(5)
  [u"line"]=>
  int(1)
  [u"column"]=>
  int(1)
  [u"proprietary"]=>
  bool(false)
  [u"id"]=>
  int(48)
  [u"attribute"]=>
  NULL
  [u"child"]=>
  array(2) {
    [0]=>
    &object(tidyNode)#3 (9) {
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
        &object(tidyNode)#6 (9) {
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
          int(57)
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
    [1]=>
    &object(tidyNode)#4 (9) {
      [u"value"]=>
      unicode(49) "<body bgcolor="#FFFFFF" alink="#000000">
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
      NULL
    }
  }
}
object(tidyNode)#2 (9) {
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
      int(57)
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
