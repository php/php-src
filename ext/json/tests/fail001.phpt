--TEST--
JSON (http://www.crockford.com/JSON/JSON_checker/test/fail*.json)
--SKIPIF--
<?php
  if (!extension_loaded('json')) die('skip: json extension not available');
?>
--FILE--
<?php
    
$tests = array('"A JSON payload should be an object or array, not a string."',
               '["Unclosed array"',
               '{unquoted_key: "keys must be quoted}',
               '["extra comma",]',
               '["double extra comma",,]',
               '[   , "<-- missing value"]',
               '["Comma after the close"],',
               '["Extra close"]]',
               '{"Extra comma": true,}',
               '{"Extra value after close": true} "misplaced quoted value"',
               '{"Illegal expression": 1 + 2}',
               '{"Illegal invocation": alert()}',
               '{"Numbers cannot have leading zeroes": 013}',
               '{"Numbers cannot be hex": 0x14}',
               '["Illegal backslash escape: \\x15"]',
               '["Illegal backslash escape: \\\'"]',
               '["Illegal backslash escape: \\017"]',
               '[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]',
               '{"Missing colon" null}',
               '{"Double colon":: null}',
               '{"Comma instead of colon", null}',
               '["Colon instead of comma": false]',
               '["Bad value", truth]',
               "['single quote']");

foreach ($tests as $test)
{
    echo 'Testing: ' . $test . "\n";
    echo "AS OBJECT\n";
    var_dump(json_decode($test));
    echo "AS ARRAY\n";
    var_dump(json_decode($test, true));
}

?>
--EXPECT--
Testing: "A JSON payload should be an object or array, not a string."
AS OBJECT
unicode(58) "A JSON payload should be an object or array, not a string."
AS ARRAY
unicode(58) "A JSON payload should be an object or array, not a string."
Testing: ["Unclosed array"
AS OBJECT
unicode(17) "["Unclosed array""
AS ARRAY
unicode(17) "["Unclosed array""
Testing: {unquoted_key: "keys must be quoted}
AS OBJECT
unicode(36) "{unquoted_key: "keys must be quoted}"
AS ARRAY
unicode(36) "{unquoted_key: "keys must be quoted}"
Testing: ["extra comma",]
AS OBJECT
unicode(16) "["extra comma",]"
AS ARRAY
unicode(16) "["extra comma",]"
Testing: ["double extra comma",,]
AS OBJECT
unicode(24) "["double extra comma",,]"
AS ARRAY
unicode(24) "["double extra comma",,]"
Testing: [   , "<-- missing value"]
AS OBJECT
unicode(26) "[   , "<-- missing value"]"
AS ARRAY
unicode(26) "[   , "<-- missing value"]"
Testing: ["Comma after the close"],
AS OBJECT
unicode(26) "["Comma after the close"],"
AS ARRAY
unicode(26) "["Comma after the close"],"
Testing: ["Extra close"]]
AS OBJECT
unicode(16) "["Extra close"]]"
AS ARRAY
unicode(16) "["Extra close"]]"
Testing: {"Extra comma": true,}
AS OBJECT
unicode(22) "{"Extra comma": true,}"
AS ARRAY
unicode(22) "{"Extra comma": true,}"
Testing: {"Extra value after close": true} "misplaced quoted value"
AS OBJECT
unicode(58) "{"Extra value after close": true} "misplaced quoted value""
AS ARRAY
unicode(58) "{"Extra value after close": true} "misplaced quoted value""
Testing: {"Illegal expression": 1 + 2}
AS OBJECT
unicode(29) "{"Illegal expression": 1 + 2}"
AS ARRAY
unicode(29) "{"Illegal expression": 1 + 2}"
Testing: {"Illegal invocation": alert()}
AS OBJECT
unicode(31) "{"Illegal invocation": alert()}"
AS ARRAY
unicode(31) "{"Illegal invocation": alert()}"
Testing: {"Numbers cannot have leading zeroes": 013}
AS OBJECT
unicode(43) "{"Numbers cannot have leading zeroes": 013}"
AS ARRAY
unicode(43) "{"Numbers cannot have leading zeroes": 013}"
Testing: {"Numbers cannot be hex": 0x14}
AS OBJECT
unicode(31) "{"Numbers cannot be hex": 0x14}"
AS ARRAY
unicode(31) "{"Numbers cannot be hex": 0x14}"
Testing: ["Illegal backslash escape: \x15"]
AS OBJECT
unicode(34) "["Illegal backslash escape: \x15"]"
AS ARRAY
unicode(34) "["Illegal backslash escape: \x15"]"
Testing: ["Illegal backslash escape: \'"]
AS OBJECT
unicode(32) "["Illegal backslash escape: \'"]"
AS ARRAY
unicode(32) "["Illegal backslash escape: \'"]"
Testing: ["Illegal backslash escape: \017"]
AS OBJECT
unicode(34) "["Illegal backslash escape: \017"]"
AS ARRAY
unicode(34) "["Illegal backslash escape: \017"]"
Testing: [[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
AS OBJECT
unicode(266) "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
AS ARRAY
unicode(266) "[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[["Too deep"]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]"
Testing: {"Missing colon" null}
AS OBJECT
unicode(22) "{"Missing colon" null}"
AS ARRAY
unicode(22) "{"Missing colon" null}"
Testing: {"Double colon":: null}
AS OBJECT
unicode(23) "{"Double colon":: null}"
AS ARRAY
unicode(23) "{"Double colon":: null}"
Testing: {"Comma instead of colon", null}
AS OBJECT
unicode(32) "{"Comma instead of colon", null}"
AS ARRAY
unicode(32) "{"Comma instead of colon", null}"
Testing: ["Colon instead of comma": false]
AS OBJECT
unicode(33) "["Colon instead of comma": false]"
AS ARRAY
unicode(33) "["Colon instead of comma": false]"
Testing: ["Bad value", truth]
AS OBJECT
unicode(20) "["Bad value", truth]"
AS ARRAY
unicode(20) "["Bad value", truth]"
Testing: ['single quote']
AS OBJECT
unicode(16) "['single quote']"
AS ARRAY
unicode(16) "['single quote']"
