--TEST--
JSON (http://www.crockford.com/JSON/JSON_checker/test/pass1.json)
--INI--
serialize_precision=-1
--SKIPIF--
<?php
if (!extension_loaded('json')) die('skip json extension not loaded');
?>
--FILE--
<?php
/* Modified to test unescaped UNICODE as keys and values.
 * Modified to test numbers with exponents without a decimal point.
 * Modified to test empty string values.
 * Modified to test a mix of integers and strings as keys.
 */

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
$obj_enc = json_encode($obj, JSON_PARTIAL_OUTPUT_ON_ERROR);
echo $obj_enc . "\n";
echo "ENCODE: FROM ARRAY\n";
$arr_enc = json_encode($arr, JSON_PARTIAL_OUTPUT_ON_ERROR);
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
  string(23) "JSON Test Pattern pass1"
  [1]=>
  object(stdClass)#%d (1) {
    ["object with 1 member"]=>
    array(1) {
      [0]=>
      string(20) "array with 1 element"
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
    ["integer"]=>
    int(1234567890)
    ["real"]=>
    float(-9876.54321)
    ["e"]=>
    float(1.23456789E-13)
    ["E"]=>
    float(1.23456789E+34)
    [""]=>
    float(INF)
    ["E no ."]=>
    float(4000000000000)
    ["zero"]=>
    int(0)
    ["one"]=>
    int(1)
    ["space"]=>
    string(1) " "
    ["quote"]=>
    string(1) """
    ["backslash"]=>
    string(1) "\"
    ["controls"]=>
    string(5) "
	"
    ["slash"]=>
    string(5) "/ & /"
    ["alpha"]=>
    string(25) "abcdefghijklmnopqrstuvwyz"
    ["ALPHA"]=>
    string(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    ["digit"]=>
    string(10) "0123456789"
    ["special"]=>
    string(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    ["hex"]=>
    string(17) "ģ䕧覫췯ꯍ"
    ["unicode"]=>
    string(18) "プレスキット"
    ["プレスキット"]=>
    string(18) "プレスキット"
    ["empty_string"]=>
    string(0) ""
    ["true"]=>
    bool(true)
    ["false"]=>
    bool(false)
    ["null"]=>
    NULL
    ["array"]=>
    array(0) {
    }
    ["object"]=>
    object(stdClass)#%d (0) {
    }
    ["123"]=>
    object(stdClass)#%d (1) {
      ["456"]=>
      object(stdClass)#%d (1) {
        ["abc"]=>
        object(stdClass)#%d (3) {
          ["789"]=>
          string(3) "def"
          ["012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(1) "5"
            [3]=>
            int(500)
          }
          ["ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(4) "five"
            [3]=>
            int(50)
            [4]=>
            string(5) "sixty"
          }
        }
      }
    }
    ["address"]=>
    string(19) "50 St. James Street"
    ["url"]=>
    string(20) "http://www.JSON.org/"
    ["comment"]=>
    string(13) "// /* <!-- --"
    ["# -- --> */"]=>
    string(1) " "
    [" s p a c e d "]=>
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
    ["compact"]=>
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
    ["jsontext"]=>
    string(49) "{"object with 1 member":["array with 1 element"]}"
    ["quotes"]=>
    string(27) "&#34; " %22 0x22 034 &#x22;"
    ["/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    string(23) "A key can be any string"
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
  string(7) "rosebud"
}
DECODE: AS ARRAY
array(14) {
  [0]=>
  string(23) "JSON Test Pattern pass1"
  [1]=>
  array(1) {
    ["object with 1 member"]=>
    array(1) {
      [0]=>
      string(20) "array with 1 element"
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
    ["integer"]=>
    int(1234567890)
    ["real"]=>
    float(-9876.54321)
    ["e"]=>
    float(1.23456789E-13)
    ["E"]=>
    float(1.23456789E+34)
    [""]=>
    float(INF)
    ["E no ."]=>
    float(4000000000000)
    ["zero"]=>
    int(0)
    ["one"]=>
    int(1)
    ["space"]=>
    string(1) " "
    ["quote"]=>
    string(1) """
    ["backslash"]=>
    string(1) "\"
    ["controls"]=>
    string(5) "
	"
    ["slash"]=>
    string(5) "/ & /"
    ["alpha"]=>
    string(25) "abcdefghijklmnopqrstuvwyz"
    ["ALPHA"]=>
    string(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    ["digit"]=>
    string(10) "0123456789"
    ["special"]=>
    string(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    ["hex"]=>
    string(17) "ģ䕧覫췯ꯍ"
    ["unicode"]=>
    string(18) "プレスキット"
    ["プレスキット"]=>
    string(18) "プレスキット"
    ["empty_string"]=>
    string(0) ""
    ["true"]=>
    bool(true)
    ["false"]=>
    bool(false)
    ["null"]=>
    NULL
    ["array"]=>
    array(0) {
    }
    ["object"]=>
    array(0) {
    }
    [123]=>
    array(1) {
      [456]=>
      array(1) {
        ["abc"]=>
        array(3) {
          [789]=>
          string(3) "def"
          ["012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(1) "5"
            [3]=>
            int(500)
          }
          ["ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(4) "five"
            [3]=>
            int(50)
            [4]=>
            string(5) "sixty"
          }
        }
      }
    }
    ["address"]=>
    string(19) "50 St. James Street"
    ["url"]=>
    string(20) "http://www.JSON.org/"
    ["comment"]=>
    string(13) "// /* <!-- --"
    ["# -- --> */"]=>
    string(1) " "
    [" s p a c e d "]=>
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
    ["compact"]=>
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
    ["jsontext"]=>
    string(49) "{"object with 1 member":["array with 1 element"]}"
    ["quotes"]=>
    string(27) "&#34; " %22 0x22 034 &#x22;"
    ["/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    string(23) "A key can be any string"
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
  string(7) "rosebud"
}
ENCODE: FROM OBJECT
["JSON Test Pattern pass1",{"object with 1 member":["array with 1 element"]},{},[],-42,true,false,null,{"integer":1234567890,"real":-9876.54321,"e":1.23456789e-13,"E":1.23456789e+34,"":0,"E no .":4000000000000,"zero":0,"one":1,"space":" ","quote":"\"","backslash":"\\","controls":"\b\f\n\r\t","slash":"\/ & \/","alpha":"abcdefghijklmnopqrstuvwyz","ALPHA":"ABCDEFGHIJKLMNOPQRSTUVWYZ","digit":"0123456789","special":"`1~!@#$%^&*()_+-={':[,]}|;.<\/>?","hex":"\u0123\u4567\u89ab\ucdef\uabcd\uef4a","unicode":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","empty_string":"","true":true,"false":false,"null":null,"array":[],"object":{},"123":{"456":{"abc":{"789":"def","012":[1,2,"5",500],"ghi":[1,2,"five",50,"sixty"]}}},"address":"50 St. James Street","url":"http:\/\/www.JSON.org\/","comment":"\/\/ \/* <!-- --","# -- --> *\/":" "," s p a c e d ":[1,2,3,4,5,6,7],"compact":[1,2,3,4,5,6,7],"jsontext":"{\"object with 1 member\":[\"array with 1 element\"]}","quotes":"&#34; \" %22 0x22 034 &#x22;","\/\\\"\ucafe\ubabe\uab98\ufcde\ubcda\uef4a\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',.\/<>?":"A key can be any string"},0.5,98.6,99.44,1066,"rosebud"]
ENCODE: FROM ARRAY
["JSON Test Pattern pass1",{"object with 1 member":["array with 1 element"]},[],[],-42,true,false,null,{"integer":1234567890,"real":-9876.54321,"e":1.23456789e-13,"E":1.23456789e+34,"":0,"E no .":4000000000000,"zero":0,"one":1,"space":" ","quote":"\"","backslash":"\\","controls":"\b\f\n\r\t","slash":"\/ & \/","alpha":"abcdefghijklmnopqrstuvwyz","ALPHA":"ABCDEFGHIJKLMNOPQRSTUVWYZ","digit":"0123456789","special":"`1~!@#$%^&*()_+-={':[,]}|;.<\/>?","hex":"\u0123\u4567\u89ab\ucdef\uabcd\uef4a","unicode":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8":"\u30d7\u30ec\u30b9\u30ad\u30c3\u30c8","empty_string":"","true":true,"false":false,"null":null,"array":[],"object":[],"123":{"456":{"abc":{"789":"def","012":[1,2,"5",500],"ghi":[1,2,"five",50,"sixty"]}}},"address":"50 St. James Street","url":"http:\/\/www.JSON.org\/","comment":"\/\/ \/* <!-- --","# -- --> *\/":" "," s p a c e d ":[1,2,3,4,5,6,7],"compact":[1,2,3,4,5,6,7],"jsontext":"{\"object with 1 member\":[\"array with 1 element\"]}","quotes":"&#34; \" %22 0x22 034 &#x22;","\/\\\"\ucafe\ubabe\uab98\ufcde\ubcda\uef4a\b\f\n\r\t`1~!@#$%^&*()_+-=[]{}|;:',.\/<>?":"A key can be any string"},0.5,98.6,99.44,1066,"rosebud"]
DECODE AGAIN: AS OBJECT
array(14) {
  [0]=>
  string(23) "JSON Test Pattern pass1"
  [1]=>
  object(stdClass)#%d (1) {
    ["object with 1 member"]=>
    array(1) {
      [0]=>
      string(20) "array with 1 element"
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
    ["integer"]=>
    int(1234567890)
    ["real"]=>
    float(-9876.54321)
    ["e"]=>
    float(1.23456789E-13)
    ["E"]=>
    float(1.23456789E+34)
    [""]=>
    int(0)
    ["E no ."]=>
    %s(4000000000000)
    ["zero"]=>
    int(0)
    ["one"]=>
    int(1)
    ["space"]=>
    string(1) " "
    ["quote"]=>
    string(1) """
    ["backslash"]=>
    string(1) "\"
    ["controls"]=>
    string(5) "
	"
    ["slash"]=>
    string(5) "/ & /"
    ["alpha"]=>
    string(25) "abcdefghijklmnopqrstuvwyz"
    ["ALPHA"]=>
    string(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    ["digit"]=>
    string(10) "0123456789"
    ["special"]=>
    string(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    ["hex"]=>
    string(17) "ģ䕧覫췯ꯍ"
    ["unicode"]=>
    string(18) "プレスキット"
    ["プレスキット"]=>
    string(18) "プレスキット"
    ["empty_string"]=>
    string(0) ""
    ["true"]=>
    bool(true)
    ["false"]=>
    bool(false)
    ["null"]=>
    NULL
    ["array"]=>
    array(0) {
    }
    ["object"]=>
    object(stdClass)#%d (0) {
    }
    ["123"]=>
    object(stdClass)#%d (1) {
      ["456"]=>
      object(stdClass)#%d (1) {
        ["abc"]=>
        object(stdClass)#%d (3) {
          ["789"]=>
          string(3) "def"
          ["012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(1) "5"
            [3]=>
            int(500)
          }
          ["ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(4) "five"
            [3]=>
            int(50)
            [4]=>
            string(5) "sixty"
          }
        }
      }
    }
    ["address"]=>
    string(19) "50 St. James Street"
    ["url"]=>
    string(20) "http://www.JSON.org/"
    ["comment"]=>
    string(13) "// /* <!-- --"
    ["# -- --> */"]=>
    string(1) " "
    [" s p a c e d "]=>
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
    ["compact"]=>
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
    ["jsontext"]=>
    string(49) "{"object with 1 member":["array with 1 element"]}"
    ["quotes"]=>
    string(27) "&#34; " %22 0x22 034 &#x22;"
    ["/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    string(23) "A key can be any string"
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
  string(7) "rosebud"
}
DECODE AGAIN: AS ARRAY
array(14) {
  [0]=>
  string(23) "JSON Test Pattern pass1"
  [1]=>
  array(1) {
    ["object with 1 member"]=>
    array(1) {
      [0]=>
      string(20) "array with 1 element"
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
    ["integer"]=>
    int(1234567890)
    ["real"]=>
    float(-9876.54321)
    ["e"]=>
    float(1.23456789E-13)
    ["E"]=>
    float(1.23456789E+34)
    [""]=>
    int(0)
    ["E no ."]=>
    %s(4000000000000)
    ["zero"]=>
    int(0)
    ["one"]=>
    int(1)
    ["space"]=>
    string(1) " "
    ["quote"]=>
    string(1) """
    ["backslash"]=>
    string(1) "\"
    ["controls"]=>
    string(5) "
	"
    ["slash"]=>
    string(5) "/ & /"
    ["alpha"]=>
    string(25) "abcdefghijklmnopqrstuvwyz"
    ["ALPHA"]=>
    string(25) "ABCDEFGHIJKLMNOPQRSTUVWYZ"
    ["digit"]=>
    string(10) "0123456789"
    ["special"]=>
    string(31) "`1~!@#$%^&*()_+-={':[,]}|;.</>?"
    ["hex"]=>
    string(17) "ģ䕧覫췯ꯍ"
    ["unicode"]=>
    string(18) "プレスキット"
    ["プレスキット"]=>
    string(18) "プレスキット"
    ["empty_string"]=>
    string(0) ""
    ["true"]=>
    bool(true)
    ["false"]=>
    bool(false)
    ["null"]=>
    NULL
    ["array"]=>
    array(0) {
    }
    ["object"]=>
    array(0) {
    }
    [123]=>
    array(1) {
      [456]=>
      array(1) {
        ["abc"]=>
        array(3) {
          [789]=>
          string(3) "def"
          ["012"]=>
          array(4) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(1) "5"
            [3]=>
            int(500)
          }
          ["ghi"]=>
          array(5) {
            [0]=>
            int(1)
            [1]=>
            int(2)
            [2]=>
            string(4) "five"
            [3]=>
            int(50)
            [4]=>
            string(5) "sixty"
          }
        }
      }
    }
    ["address"]=>
    string(19) "50 St. James Street"
    ["url"]=>
    string(20) "http://www.JSON.org/"
    ["comment"]=>
    string(13) "// /* <!-- --"
    ["# -- --> */"]=>
    string(1) " "
    [" s p a c e d "]=>
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
    ["compact"]=>
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
    ["jsontext"]=>
    string(49) "{"object with 1 member":["array with 1 element"]}"
    ["quotes"]=>
    string(27) "&#34; " %22 0x22 034 &#x22;"
    ["/\"쫾몾ꮘﳞ볚
	`1~!@#$%^&*()_+-=[]{}|;:',./<>?"]=>
    string(23) "A key can be any string"
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
  string(7) "rosebud"
}
