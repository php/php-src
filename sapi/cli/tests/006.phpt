--TEST--
show information about extension
--SKIPIF--
<?php 
include "skipif.inc"; 
if (!extension_loaded("reflection") || !extension_loaded("session")) {
	die("skip reflection and session extensions required");
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
string(%d) "Extension [ <persistent> extension #%d pcre version %s ] {

  - INI {
    Entry [ pcre.backtrack_limit <ALL> ]
      Current = '%d'
    }
    Entry [ pcre.recursion_limit <ALL> ]
      Current = '%d'
    }
    Entry [ pcre.jit <ALL> ]
      Current = '%d'
    }
  }

  - Constants [15] {
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
    Constant [ integer PREG_JIT_STACKLIMIT_ERROR ] { 6 }
    Constant [ string PCRE_VERSION ] { %s }
  }

  - Functions {
    Function [ <internal:pcre> function preg_match ] {

      - Parameters [5] {
        Parameter #0 [ <required> $pattern ]
        Parameter #1 [ <required> $subject ]
        Parameter #2 [ <optional> &$subpatterns ]
        Parameter #3 [ <optional> $flags ]
        Parameter #4 [ <optional> $offset ]
      }
    }
    Function [ <internal:pcre> function preg_match_all ] {

      - Parameters [5] {
        Parameter #0 [ <required> $pattern ]
        Parameter #1 [ <required> $subject ]
        Parameter #2 [ <optional> &$subpatterns ]
        Parameter #3 [ <optional> $flags ]
        Parameter #4 [ <optional> $offset ]
      }
    }
    Function [ <internal:pcre> function preg_replace ] {

      - Parameters [5] {
        Parameter #0 [ <required> $regex ]
        Parameter #1 [ <required> $replace ]
        Parameter #2 [ <required> $subject ]
        Parameter #3 [ <optional> $limit ]
        Parameter #4 [ <optional> &$count ]
      }
    }
    Function [ <internal:pcre> function preg_replace_callback ] {

      - Parameters [5] {
        Parameter #0 [ <required> $regex ]
        Parameter #1 [ <required> $callback ]
        Parameter #2 [ <required> $subject ]
        Parameter #3 [ <optional> $limit ]
        Parameter #4 [ <optional> &$count ]
      }
    }
    Function [ <internal:pcre> function preg_replace_callback_array ] {

      - Parameters [4] {
        Parameter #0 [ <required> $pattern ]
        Parameter #1 [ <required> $subject ]
        Parameter #2 [ <optional> $limit ]
        Parameter #3 [ <optional> &$count ]
      }
    }
    Function [ <internal:pcre> function preg_filter ] {

      - Parameters [5] {
        Parameter #0 [ <required> $regex ]
        Parameter #1 [ <required> $replace ]
        Parameter #2 [ <required> $subject ]
        Parameter #3 [ <optional> $limit ]
        Parameter #4 [ <optional> &$count ]
      }
    }
    Function [ <internal:pcre> function preg_split ] {

      - Parameters [4] {
        Parameter #0 [ <required> $pattern ]
        Parameter #1 [ <required> $subject ]
        Parameter #2 [ <optional> $limit ]
        Parameter #3 [ <optional> $flags ]
      }
    }
    Function [ <internal:pcre> function preg_quote ] {

      - Parameters [2] {
        Parameter #0 [ <required> $str ]
        Parameter #1 [ <optional> $delim_char ]
      }
    }
    Function [ <internal:pcre> function preg_grep ] {

      - Parameters [3] {
        Parameter #0 [ <required> $regex ]
        Parameter #1 [ <required> $input ]
        Parameter #2 [ <optional> $flags ]
      }
    }
    Function [ <internal:pcre> function preg_last_error ] {

      - Parameters [0] {
      }
    }
  }
}

"
Done
