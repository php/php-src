--TEST--
show information about extension
--SKIPIF--
<?php 
include "skipif.inc"; 
if (!extension_loaded("reflection")) {
	die("skip");
}
?>
--INI--
date.timezone=
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`$php -n --re unknown`);
var_dump(`$php -n --re ""`);
var_dump(`$php -n --re pcre`);

echo "Done\n";
?>
--EXPECTF--	
string(44) "Exception: Extension unknown does not exist
"
string(37) "Exception: Extension  does not exist
"
string(%d) "Extension [ <persistent> extension #%d pcre version <no_version> ] {

  - INI {
    Entry [ pcre.backtrack_limit <ALL> ]
      Current = '%d'
    }
    Entry [ pcre.recursion_limit <ALL> ]
      Current = '%d'
    }
  }

  - Constants [14] {
    Constant [ integer PREG_PATTERN_ORDER ] { 1 }
    Constant [ integer PREG_SET_ORDER ] { 2 }
    Constant [ integer PREG_OFFSET_CAPTURE ] { 256 }
    Constant [ integer PREG_SPLIT_NO_EMPTY ] { 1 }
    Constant [ integer PREG_SPLIT_DELIM_CAPTURE ] { 2 }
    Constant [ integer PREG_SPLIT_OFFSET_CAPTURE ] { 4 }
    Constant [ integer PREG_GREP_INVERT ] { 1 }
    Constant [ integer PREG_NO_ERROR ] { 0 }
    Constant [ integer PREG_INTERNAL_ERROR ] { 1 }
    Constant [ integer PREG_BACKTRACK_LIMIT_ERROR ] { 2 }
    Constant [ integer PREG_RECURSION_LIMIT_ERROR ] { 3 }
    Constant [ integer PREG_BAD_UTF8_ERROR ] { 4 }
    Constant [ integer PREG_BAD_UTF8_OFFSET_ERROR ] { 5 }
    Constant [ string PCRE_VERSION ] { %s }
  }

  - Functions {
    Function [ <internal:pcre> function preg_match ] {

      - Parameters [3] {
        Parameter #0 [ <required> $param0 ]
        Parameter #1 [ <required> $param1 ]
        Parameter #2 [ <required> &$param2 ]
      }
    }
    Function [ <internal:pcre> function preg_match_all ] {

      - Parameters [3] {
        Parameter #0 [ <required> $param0 ]
        Parameter #1 [ <required> $param1 ]
        Parameter #2 [ <required> &$param2 ]
      }
    }
    Function [ <internal:pcre> function preg_replace ] {

      - Parameters [5] {
        Parameter #0 [ <required> $param0 ]
        Parameter #1 [ <required> $param1 ]
        Parameter #2 [ <required> $param2 ]
        Parameter #3 [ <required> $param3 ]
        Parameter #4 [ <required> &$param4 ]
      }
    }
    Function [ <internal:pcre> function preg_replace_callback ] {

      - Parameters [5] {
        Parameter #0 [ <required> $param0 ]
        Parameter #1 [ <required> $param1 ]
        Parameter #2 [ <required> $param2 ]
        Parameter #3 [ <required> $param3 ]
        Parameter #4 [ <required> &$param4 ]
      }
    }
    Function [ <internal:pcre> function preg_split ] {
    }
    Function [ <internal:pcre> function preg_quote ] {
    }
    Function [ <internal:pcre> function preg_grep ] {
    }
    Function [ <internal:pcre> function preg_last_error ] {
    }
  }
}

"
Done
