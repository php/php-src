--TEST--
show information about extension
--SKIPIF--
<?php
include "skipif.inc";
if (!extension_loaded("session")) {
    die("skip session extension required");
}
if (PCRE_JIT_SUPPORT == false) {
    die ("skip not pcre jit support builtin");
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
string(46) "Exception: Extension "unknown" does not exist
"
string(39) "Exception: Extension "" does not exist
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

  - Constants [19] {
    Constant [ int PREG_PATTERN_ORDER ] { 1 }
    Constant [ int PREG_SET_ORDER ] { 2 }
    Constant [ int PREG_OFFSET_CAPTURE ] { 256 }
    Constant [ int PREG_UNMATCHED_AS_NULL ] { 512 }
    Constant [ int PREG_SPLIT_NO_EMPTY ] { 1 }
    Constant [ int PREG_SPLIT_DELIM_CAPTURE ] { 2 }
    Constant [ int PREG_SPLIT_OFFSET_CAPTURE ] { 4 }
    Constant [ int PREG_GREP_INVERT ] { 1 }
    Constant [ int PREG_NO_ERROR ] { 0 }
    Constant [ int PREG_INTERNAL_ERROR ] { 1 }
    Constant [ int PREG_BACKTRACK_LIMIT_ERROR ] { 2 }
    Constant [ int PREG_RECURSION_LIMIT_ERROR ] { 3 }
    Constant [ int PREG_BAD_UTF8_ERROR ] { 4 }
    Constant [ int PREG_BAD_UTF8_OFFSET_ERROR ] { 5 }
    Constant [ int PREG_JIT_STACKLIMIT_ERROR ] { 6 }
    Constant [ string PCRE_VERSION ] { %s }
    Constant [ int PCRE_VERSION_MAJOR ] { %d }
    Constant [ int PCRE_VERSION_MINOR ] { %d }
    Constant [ bool PCRE_JIT_SUPPORT ] { %d }
  }

  - Functions {
    Function [ <internal:pcre> function preg_match ] {

      - Parameters [5] {
        Parameter #0 [ <required> string $pattern ]
        Parameter #1 [ <required> string $subject ]
        Parameter #2 [ <optional> &$matches = null ]
        Parameter #3 [ <optional> int $flags = 0 ]
        Parameter #4 [ <optional> int $offset = 0 ]
      }
      - Return [ int|false ]
    }
    Function [ <internal:pcre> function preg_match_all ] {

      - Parameters [5] {
        Parameter #0 [ <required> string $pattern ]
        Parameter #1 [ <required> string $subject ]
        Parameter #2 [ <optional> &$matches = null ]
        Parameter #3 [ <optional> int $flags = 0 ]
        Parameter #4 [ <optional> int $offset = 0 ]
      }
      - Return [ int|false ]
    }
    Function [ <internal:pcre> function preg_replace ] {

      - Parameters [5] {
        Parameter #0 [ <required> array|string $pattern ]
        Parameter #1 [ <required> array|string $replacement ]
        Parameter #2 [ <required> array|string $subject ]
        Parameter #3 [ <optional> int $limit = -1 ]
        Parameter #4 [ <optional> &$count = null ]
      }
      - Return [ array|string|null ]
    }
    Function [ <internal:pcre> function preg_filter ] {

      - Parameters [5] {
        Parameter #0 [ <required> array|string $pattern ]
        Parameter #1 [ <required> array|string $replacement ]
        Parameter #2 [ <required> array|string $subject ]
        Parameter #3 [ <optional> int $limit = -1 ]
        Parameter #4 [ <optional> &$count = null ]
      }
      - Return [ array|string|null ]
    }
    Function [ <internal:pcre> function preg_replace_callback ] {

      - Parameters [6] {
        Parameter #0 [ <required> array|string $pattern ]
        Parameter #1 [ <required> callable $callback ]
        Parameter #2 [ <required> array|string $subject ]
        Parameter #3 [ <optional> int $limit = -1 ]
        Parameter #4 [ <optional> &$count = null ]
        Parameter #5 [ <optional> int $flags = 0 ]
      }
      - Return [ array|string|null ]
    }
    Function [ <internal:pcre> function preg_replace_callback_array ] {

      - Parameters [5] {
        Parameter #0 [ <required> array $pattern ]
        Parameter #1 [ <required> array|string $subject ]
        Parameter #2 [ <optional> int $limit = -1 ]
        Parameter #3 [ <optional> &$count = null ]
        Parameter #4 [ <optional> int $flags = 0 ]
      }
      - Return [ array|string|null ]
    }
    Function [ <internal:pcre> function preg_split ] {

      - Parameters [4] {
        Parameter #0 [ <required> string $pattern ]
        Parameter #1 [ <required> string $subject ]
        Parameter #2 [ <optional> int $limit = -1 ]
        Parameter #3 [ <optional> int $flags = 0 ]
      }
      - Return [ array|false ]
    }
    Function [ <internal:pcre> function preg_quote ] {

      - Parameters [2] {
        Parameter #0 [ <required> string $str ]
        Parameter #1 [ <optional> ?string $delimiter = null ]
      }
      - Return [ string ]
    }
    Function [ <internal:pcre> function preg_grep ] {

      - Parameters [3] {
        Parameter #0 [ <required> string $pattern ]
        Parameter #1 [ <required> array $array ]
        Parameter #2 [ <optional> int $flags = 0 ]
      }
      - Return [ array|false ]
    }
    Function [ <internal:pcre> function preg_last_error ] {

      - Parameters [0] {
      }
      - Return [ int ]
    }
    Function [ <internal:pcre> function preg_last_error_msg ] {

      - Parameters [0] {
      }
      - Return [ string ]
    }
  }
}

"
Done
