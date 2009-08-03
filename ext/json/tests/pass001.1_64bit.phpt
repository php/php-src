--TEST--
JSON (http://www.crockford.com/JSON/JSON_checker/test/pass1.json)
--INI--
precision=14
--SKIPIF--
<?php
  if (!extension_loaded('json')) die('skip: json extension not available');
  if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Modified to test unescaped UNICODE as keys and values.
 * Modified to test numbers with exponents without a decimal point.
 * Modified to test empty string values.
 * Modified to test a mix of integers and strings as keys.
 */
// Expect warnings about INF.
ini_set("error_reporting", E_ALL & ~E_WARNING);

$test = "
[
    \"JSON Test Pattern pass1\",
    {\"object with 1 member\":[\"array with 1 element\"]},
    {},
    [],
    -42,
    true,
    false,
    null,
    {
        \"integer\": 1234567890,
        \"real\": -9876.543210,
        \"e\": 0.123456789e-12,
        \"E\": 1.234567890E+34,
        \"\":  23456789012E666,
        \"E no .\":  4E12,
        \"zero\": 0,
        \"one\": 1,
        \"space\": \" \",
        \"quote\": \"\\\"\",
        \"backslash\": \"\\\\\",
        \"controls\": \"\\b\\f\\n\\r\\t\",
        \"slash\": \"/ & \\/\",
        \"alpha\": \"abcdefghijklmnopqrstuvwyz\",
        \"ALPHA\": \"ABCDEFGHIJKLMNOPQRSTUVWYZ\",
        \"digit\": \"0123456789\",
        \"special\": \"`1~!@#$%^&*()_+-={':[,]}|;.</>?\",
        \"hex\": \"\\u0123\\u4567\\u89AB\\uCDEF\\uabcd\\uef4A\",
        \"unicode\": \"\\u30d7\\u30ec\\u30b9\\u30ad\\u30c3\\u30c8\",
        \"プレスキット\": \"プレスキット\",
        \"empty_string\": \"\",
        \"true\": true,
        \"false\": false,
        \"null\": null,
        \"array\":[  ],
        \"object\":{  },
        \"123\":{\"456\":{\"abc\":{\"789\":\"def\",\"012\":[1,2,\"5\",500],\"ghi\":[1,2,\"five\",50,\"sixty\"]}}},
        \"address\": \"50 St. James Street\",
        \"url\": \"http://www.JSON.org/\",
        \"comment\": \"// /* <!-- --\",
        \"# -- --> */\": \" \",
        \" s p a c e d \" :[1,2 , 3

,

4 , 5        ,          6           ,7        ],
        \"compact\": [1,2,3,4,5,6,7],
        \"jsontext\": \"{\\\"object with 1 member\\\":[\\\"array with 1 element\\\"]}\",
        \"quotes\": \"&#34; \\u0022 %22 0x22 034 &#x22;\",
        \"\\/\\\\\\\"\\uCAFE\\uBABE\\uAB98\\uFCDE\\ubcda\\uef4A\\b\\f\\n\\r\\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?\"
: \"A key can be any string\"
    },
    0.5 ,98.6
,
99.44
,

1066


,\"rosebud\"]
";

echo 'Testing: ' . $test . "\n";
echo "DECODE: AS OBJECT\n";
$obj = json_decode($test);
var_dump($obj);
echo "DECODE: AS ARRAY\n";
$arr = json_decode($test, true);
var_dump($arr);

echo "ENCODE: FROM OBJECT\n";
$obj_enc = json_encode($obj);
echo $obj_enc . "\n";
echo "ENCODE: FROM ARRAY\n";
$arr_enc = json_encode($arr);
echo $arr_enc . "\n";

echo "DECODE AGAIN: AS OBJECT\n";
$obj = json_decode($obj_enc);
var_dump($obj);
echo "DECODE AGAIN: AS ARRAY\n";
$arr = json_decode($arr_enc, true);
var_dump($arr);

?>
--EXPECTF--
Testing: 
[
    "JSON Test Pattern pass1",
    {"object with 1 member":["array with 1 element"]},
    {},
    [],
    -42,
    true,
    false,
    null,
    {
        "integer": 1234567890,
        "real": -9876.543210,
        "e": 0.123456789e-12,
        "E": 1.234567890E+34,
        "":  23456789012E666,
        "E no .":  4E12,
        "zero": 0,
        "one": 1,
        "space": " ",
        "quote": "\"",
        "backslash": "\\",
        "controls": "\b\f\n\r\t",
        "slash": "/ & \/",
        "alpha": "abcdefghijklmnopqrstuvwyz",
        "ALPHA": "ABCDEFGHIJKLMNOPQRSTUVWYZ",
        "digit": "0123456789",
        "special": "`1~!@#$%^&*()_+-={':[,]}|;.</>?",
        "hex": "\u0123\u4567\u89AB\uCDEF\uabcd\uef4A",
        "unicode": "\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8",
        "プレスキット": "プレスキット",
        "empty_string": "",
        "true": true,
        "false": false,
        "null": null,
        "array":[  ],
        "object":{  },
        "123":{"456":{"abc":{"789":"def","012":[1,2,"5",500],"ghi":[1,2,"five",50,"sixty"]}}},
        "address": "50 St. James Street",
        "url": "http://www.JSON.org/",
        "comment": "// /* <!-- --",
        "# -- --> */": " ",
        " s p a c e d " :[1,2 , 3

,

4 , 5        ,          6           ,7        ],
        "compact": [1,2,3,4,5,6,7],
        "jsontext": "{\"object with 1 member\":[\"array with 1 element\"]}",
        "quotes": "&#34; \u0022 %22 0x22 034 &#x22;",
        "\/\\\"\uCAFE\uBABE\uAB98\uFCDE\ubcda\uef4A\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',./<>?"
: "A key can be any string"
    },
    0.5 ,98.6
,
99.44
,

1066


,"rosebud"]

DECODE: AS OBJECT
array(14) {
  [0]=>
  unicode(23) "JSON Test Pattern pass1"
  [1]=>
  object(stdClass)#%d (1) {
    [u"object with 1 member"]=>
    array(1) {
      [0]=>
      unicode(20) "array with 1 element"
    }
  }
  [2]=>
  object(stdClass)#%d (0) {
  }
  [3]=>
  array(0) {
  }
  [4]=>
  int(-42)
  [5]=>
  bool(true)
  [6]=>
  bool(false)
  [7]=>
  NULL
  [8]=>
  object(stdClass)#%d (36) {
    [u"integer"]=>
    int(1234567890)
    [u"real"]=>
    float(-9876.54321)
    [u"e"]=>
    float(1.23456789E-13)
    [u"E"]=>
    float(1.23456789E+34)
    [u"_empty_"]=>
    float(INF)
    [u"E no ."]=>
    float(4000000000000)
    [u"zero"]=>
    int(0)
    [u"one"]=>
    int(1)
    [u"space"]=>
    unicode(1) " "
    [u"quote"]=>
    unicode(1) """
    [u"backslash"]=>
    unicode(1) "\"
    [u"controls"]=>
    unicode(5) "
	"
    [u"slash"]=>
    unicode(5) "/ & /"
    [u"alpha"]=>
    unicode(25) "abcdefghijklmnopqrstuvwyz"
    [u"ALPHA"]=>
    unicode(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    [u"digit"]=>
    unicode(10) "0123456789"
    [u"special"]=>
    unicode(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    [u"hex"]=>
    unicode(6) "ģ䕧覫췯ꯍ"
    [u"unicode"]=>
    unicode(6) "プレスキット"
    [u"プレスキット"]=>
    unicode(6) "プレスキット"
    [u"empty_string"]=>
    unicode(0) ""
    [u"true"]=>
    bool(true)
    [u"false"]=>
    bool(false)
    [u"null"]=>
    NULL
    [u"array"]=>
    array(0) {
    }
    [u"object"]=>
    object(stdClass)#%d (0) {
    }
    [u"123"]=>
    object(stdClass)#%d (1) {
      [u"456"]=>
      object(stdClass)#%d (1) {
        [u"abc"]=>
        object(stdClass)#%d (3) {
          [u"789"]=>
          unicode(3) "def"
          [u"012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(1) "5"
            [3]=>
            int(500)
          }
          [u"ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(4) "five"
            [3]=>
            int(50)
            [4]=>
            unicode(5) "sixty"
          }
        }
      }
    }
    [u"address"]=>
    unicode(19) "50 St. James Street"
    [u"url"]=>
    unicode(20) "http://www.JSON.org/"
    [u"comment"]=>
    unicode(13) "// /* <!-- --"
    [u"# -- --> */"]=>
    unicode(1) " "
    [u" s p a c e d "]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"compact"]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"jsontext"]=>
    unicode(49) "{"object with 1 member":["array with 1 element"]}"
    [u"quotes"]=>
    unicode(27) "&#34; " %22 0x22 034 &#x22;"
    [u"/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    unicode(23) "A key can be any string"
  }
  [9]=>
  float(0.5)
  [10]=>
  float(98.6)
  [11]=>
  float(99.44)
  [12]=>
  int(1066)
  [13]=>
  unicode(7) "rosebud"
}
DECODE: AS ARRAY
array(14) {
  [0]=>
  unicode(23) "JSON Test Pattern pass1"
  [1]=>
  array(1) {
    [u"object with 1 member"]=>
    array(1) {
      [0]=>
      unicode(20) "array with 1 element"
    }
  }
  [2]=>
  array(0) {
  }
  [3]=>
  array(0) {
  }
  [4]=>
  int(-42)
  [5]=>
  bool(true)
  [6]=>
  bool(false)
  [7]=>
  NULL
  [8]=>
  array(36) {
    [u"integer"]=>
    int(1234567890)
    [u"real"]=>
    float(-9876.54321)
    [u"e"]=>
    float(1.23456789E-13)
    [u"E"]=>
    float(1.23456789E+34)
    [u""]=>
    float(INF)
    [u"E no ."]=>
    float(4000000000000)
    [u"zero"]=>
    int(0)
    [u"one"]=>
    int(1)
    [u"space"]=>
    unicode(1) " "
    [u"quote"]=>
    unicode(1) """
    [u"backslash"]=>
    unicode(1) "\"
    [u"controls"]=>
    unicode(5) "
	"
    [u"slash"]=>
    unicode(5) "/ & /"
    [u"alpha"]=>
    unicode(25) "abcdefghijklmnopqrstuvwyz"
    [u"ALPHA"]=>
    unicode(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    [u"digit"]=>
    unicode(10) "0123456789"
    [u"special"]=>
    unicode(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    [u"hex"]=>
    unicode(6) "ģ䕧覫췯ꯍ"
    [u"unicode"]=>
    unicode(6) "プレスキット"
    [u"プレスキット"]=>
    unicode(6) "プレスキット"
    [u"empty_string"]=>
    unicode(0) ""
    [u"true"]=>
    bool(true)
    [u"false"]=>
    bool(false)
    [u"null"]=>
    NULL
    [u"array"]=>
    array(0) {
    }
    [u"object"]=>
    array(0) {
    }
    [123]=>
    array(1) {
      [456]=>
      array(1) {
        [u"abc"]=>
        array(3) {
          [789]=>
          unicode(3) "def"
          [u"012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(1) "5"
            [3]=>
            int(500)
          }
          [u"ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(4) "five"
            [3]=>
            int(50)
            [4]=>
            unicode(5) "sixty"
          }
        }
      }
    }
    [u"address"]=>
    unicode(19) "50 St. James Street"
    [u"url"]=>
    unicode(20) "http://www.JSON.org/"
    [u"comment"]=>
    unicode(13) "// /* <!-- --"
    [u"# -- --> */"]=>
    unicode(1) " "
    [u" s p a c e d "]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"compact"]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"jsontext"]=>
    unicode(49) "{"object with 1 member":["array with 1 element"]}"
    [u"quotes"]=>
    unicode(27) "&#34; " %22 0x22 034 &#x22;"
    [u"/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    unicode(23) "A key can be any string"
  }
  [9]=>
  float(0.5)
  [10]=>
  float(98.6)
  [11]=>
  float(99.44)
  [12]=>
  int(1066)
  [13]=>
  unicode(7) "rosebud"
}
ENCODE: FROM OBJECT
["JSON Test Pattern pass1",{"object with 1 member":["array with 1 element"]},{},[],-42,true,false,null,{"integer":1234567890,"real":-9876.54321,"e":1.23456789e-13,"E":1.23456789e+34,"_empty_":0,"E no .":4000000000000,"zero":0,"one":1,"space":" ","quote":"\"","backslash":"\\","controls":"\b\f\n\r\t","slash":"\/ & \/","alpha":"abcdefghijklmnopqrstuvwyz","ALPHA":"ABCDEFGHIJKLMNOPQRSTUVWYZ","digit":"0123456789","special":"`1~!@#$%^&*()_+-={':[,]}|;.<\/>?","hex":"\u0123\u4567\u89ab\ucdef\uabcd\uef4a","unicode":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","empty_string":"","true":true,"false":false,"null":null,"array":[],"object":{},"123":{"456":{"abc":{"789":"def","012":[1,2,"5",500],"ghi":[1,2,"five",50,"sixty"]}}},"address":"50 St. James Street","url":"http:\/\/www.JSON.org\/","comment":"\/\/ \/* <!-- --","# -- --> *\/":" "," s p a c e d ":[1,2,3,4,5,6,7],"compact":[1,2,3,4,5,6,7],"jsontext":"{\"object with 1 member\":[\"array with 1 element\"]}","quotes":"&#34; \" %22 0x22 034 &#x22;","\/\\\"\ucafe\ubabe\uab98\ufcde\ubcda\uef4a\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',.\/<>?":"A key can be any string"},0.5,98.6,99.44,1066,"rosebud"]
ENCODE: FROM ARRAY
["JSON Test Pattern pass1",{"object with 1 member":["array with 1 element"]},[],[],-42,true,false,null,{"integer":1234567890,"real":-9876.54321,"e":1.23456789e-13,"E":1.23456789e+34,"":0,"E no .":4000000000000,"zero":0,"one":1,"space":" ","quote":"\"","backslash":"\\","controls":"\b\f\n\r\t","slash":"\/ & \/","alpha":"abcdefghijklmnopqrstuvwyz","ALPHA":"ABCDEFGHIJKLMNOPQRSTUVWYZ","digit":"0123456789","special":"`1~!@#$%^&*()_+-={':[,]}|;.<\/>?","hex":"\u0123\u4567\u89ab\ucdef\uabcd\uef4a","unicode":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","empty_string":"","true":true,"false":false,"null":null,"array":[],"object":[],"123":{"456":{"abc":{"789":"def","012":[1,2,"5",500],"ghi":[1,2,"five",50,"sixty"]}}},"address":"50 St. James Street","url":"http:\/\/www.JSON.org\/","comment":"\/\/ \/* <!-- --","# -- --> *\/":" "," s p a c e d ":[1,2,3,4,5,6,7],"compact":[1,2,3,4,5,6,7],"jsontext":"{\"object with 1 member\":[\"array with 1 element\"]}","quotes":"&#34; \" %22 0x22 034 &#x22;","\/\\\"\ucafe\ubabe\uab98\ufcde\ubcda\uef4a\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',.\/<>?":"A key can be any string"},0.5,98.6,99.44,1066,"rosebud"]
DECODE AGAIN: AS OBJECT
array(14) {
  [0]=>
  unicode(23) "JSON Test Pattern pass1"
  [1]=>
  object(stdClass)#%d (1) {
    [u"object with 1 member"]=>
    array(1) {
      [0]=>
      unicode(20) "array with 1 element"
    }
  }
  [2]=>
  object(stdClass)#%d (0) {
  }
  [3]=>
  array(0) {
  }
  [4]=>
  int(-42)
  [5]=>
  bool(true)
  [6]=>
  bool(false)
  [7]=>
  NULL
  [8]=>
  object(stdClass)#%d (36) {
    [u"integer"]=>
    int(1234567890)
    [u"real"]=>
    float(-9876.54321)
    [u"e"]=>
    float(1.23456789E-13)
    [u"E"]=>
    float(1.23456789E+34)
    [u"_empty_"]=>
    int(0)
    [u"E no ."]=>
    int(4000000000000)
    [u"zero"]=>
    int(0)
    [u"one"]=>
    int(1)
    [u"space"]=>
    unicode(1) " "
    [u"quote"]=>
    unicode(1) """
    [u"backslash"]=>
    unicode(1) "\"
    [u"controls"]=>
    unicode(5) "
	"
    [u"slash"]=>
    unicode(5) "/ & /"
    [u"alpha"]=>
    unicode(25) "abcdefghijklmnopqrstuvwyz"
    [u"ALPHA"]=>
    unicode(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    [u"digit"]=>
    unicode(10) "0123456789"
    [u"special"]=>
    unicode(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    [u"hex"]=>
    unicode(6) "ģ䕧覫췯ꯍ"
    [u"unicode"]=>
    unicode(6) "プレスキット"
    [u"プレスキット"]=>
    unicode(6) "プレスキット"
    [u"empty_string"]=>
    unicode(0) ""
    [u"true"]=>
    bool(true)
    [u"false"]=>
    bool(false)
    [u"null"]=>
    NULL
    [u"array"]=>
    array(0) {
    }
    [u"object"]=>
    object(stdClass)#%d (0) {
    }
    [u"123"]=>
    object(stdClass)#%d (1) {
      [u"456"]=>
      object(stdClass)#%d (1) {
        [u"abc"]=>
        object(stdClass)#%d (3) {
          [u"789"]=>
          unicode(3) "def"
          [u"012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(1) "5"
            [3]=>
            int(500)
          }
          [u"ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(4) "five"
            [3]=>
            int(50)
            [4]=>
            unicode(5) "sixty"
          }
        }
      }
    }
    [u"address"]=>
    unicode(19) "50 St. James Street"
    [u"url"]=>
    unicode(20) "http://www.JSON.org/"
    [u"comment"]=>
    unicode(13) "// /* <!-- --"
    [u"# -- --> */"]=>
    unicode(1) " "
    [u" s p a c e d "]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"compact"]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"jsontext"]=>
    unicode(49) "{"object with 1 member":["array with 1 element"]}"
    [u"quotes"]=>
    unicode(27) "&#34; " %22 0x22 034 &#x22;"
    [u"/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    unicode(23) "A key can be any string"
  }
  [9]=>
  float(0.5)
  [10]=>
  float(98.6)
  [11]=>
  float(99.44)
  [12]=>
  int(1066)
  [13]=>
  unicode(7) "rosebud"
}
DECODE AGAIN: AS ARRAY
array(14) {
  [0]=>
  unicode(23) "JSON Test Pattern pass1"
  [1]=>
  array(1) {
    [u"object with 1 member"]=>
    array(1) {
      [0]=>
      unicode(20) "array with 1 element"
    }
  }
  [2]=>
  array(0) {
  }
  [3]=>
  array(0) {
  }
  [4]=>
  int(-42)
  [5]=>
  bool(true)
  [6]=>
  bool(false)
  [7]=>
  NULL
  [8]=>
  array(36) {
    [u"integer"]=>
    int(1234567890)
    [u"real"]=>
    float(-9876.54321)
    [u"e"]=>
    float(1.23456789E-13)
    [u"E"]=>
    float(1.23456789E+34)
    [u""]=>
    int(0)
    [u"E no ."]=>
    int(4000000000000)
    [u"zero"]=>
    int(0)
    [u"one"]=>
    int(1)
    [u"space"]=>
    unicode(1) " "
    [u"quote"]=>
    unicode(1) """
    [u"backslash"]=>
    unicode(1) "\"
    [u"controls"]=>
    unicode(5) "
	"
    [u"slash"]=>
    unicode(5) "/ & /"
    [u"alpha"]=>
    unicode(25) "abcdefghijklmnopqrstuvwyz"
    [u"ALPHA"]=>
    unicode(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    [u"digit"]=>
    unicode(10) "0123456789"
    [u"special"]=>
    unicode(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    [u"hex"]=>
    unicode(6) "ģ䕧覫췯ꯍ"
    [u"unicode"]=>
    unicode(6) "プレスキット"
    [u"プレスキット"]=>
    unicode(6) "プレスキット"
    [u"empty_string"]=>
    unicode(0) ""
    [u"true"]=>
    bool(true)
    [u"false"]=>
    bool(false)
    [u"null"]=>
    NULL
    [u"array"]=>
    array(0) {
    }
    [u"object"]=>
    array(0) {
    }
    [123]=>
    array(1) {
      [456]=>
      array(1) {
        [u"abc"]=>
        array(3) {
          [789]=>
          unicode(3) "def"
          [u"012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(1) "5"
            [3]=>
            int(500)
          }
          [u"ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            unicode(4) "five"
            [3]=>
            int(50)
            [4]=>
            unicode(5) "sixty"
          }
        }
      }
    }
    [u"address"]=>
    unicode(19) "50 St. James Street"
    [u"url"]=>
    unicode(20) "http://www.JSON.org/"
    [u"comment"]=>
    unicode(13) "// /* <!-- --"
    [u"# -- --> */"]=>
    unicode(1) " "
    [u" s p a c e d "]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"compact"]=>
    array(7) {
      [0]=>
      int(1)
      [1]=>
      int(2)
      [2]=>
      int(3)
      [3]=>
      int(4)
      [4]=>
      int(5)
      [5]=>
      int(6)
      [6]=>
      int(7)
    }
    [u"jsontext"]=>
    unicode(49) "{"object with 1 member":["array with 1 element"]}"
    [u"quotes"]=>
    unicode(27) "&#34; " %22 0x22 034 &#x22;"
    [u"/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    unicode(23) "A key can be any string"
  }
  [9]=>
  float(0.5)
  [10]=>
  float(98.6)
  [11]=>
  float(99.44)
  [12]=>
  int(1066)
  [13]=>
  unicode(7) "rosebud"
}
