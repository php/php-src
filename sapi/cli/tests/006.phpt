--TEST--
show information about extension
--SKIPIF--
<?php 
include "skipif.inc"; 
if (!extension_loaded("reflection")) {
	die("skip");
}
?>
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');

var_dump(`"$php" -n --re unknown`);
var_dump(`"$php" -n --re ""`);
var_dump(`"$php" -n --re date`);

echo "Done\n";
?>
--EXPECTF--	
string(44) "Exception: Extension unknown does not exist
"
string(37) "Exception: Extension  does not exist
"
string(9650) "Extension [ <persistent> extension #20 date version 6.0.0-dev ] {

  - Dependencies {
    Dependency [ session (Optional) ]
  }

  - INI {
    Entry [ date.timezone <ALL> ]
      Current = ''
    }
    Entry [ date.default_latitude <ALL> ]
      Current = '31.7667'
    }
    Entry [ date.default_longitude <ALL> ]
      Current = '35.2333'
    }
    Entry [ date.sunset_zenith <ALL> ]
      Current = '90.583333'
    }
    Entry [ date.sunrise_zenith <ALL> ]
      Current = '90.583333'
    }
  }

  - Constants [14] {
    Constant [ string DATE_ATOM ] { Y-m-d\TH:i:sP }
    Constant [ string DATE_COOKIE ] { l, d-M-y H:i:s T }
    Constant [ string DATE_ISO8601 ] { Y-m-d\TH:i:sO }
    Constant [ string DATE_RFC822 ] { D, d M y H:i:s O }
    Constant [ string DATE_RFC850 ] { l, d-M-y H:i:s T }
    Constant [ string DATE_RFC1036 ] { D, d M y H:i:s O }
    Constant [ string DATE_RFC1123 ] { D, d M Y H:i:s O }
    Constant [ string DATE_RFC2822 ] { D, d M Y H:i:s O }
    Constant [ string DATE_RFC3339 ] { Y-m-d\TH:i:sP }
    Constant [ string DATE_RSS ] { D, d M Y H:i:s O }
    Constant [ string DATE_W3C ] { Y-m-d\TH:i:sP }
    Constant [ integer SUNFUNCS_RET_TIMESTAMP ] { 0 }
    Constant [ integer SUNFUNCS_RET_STRING ] { 1 }
    Constant [ integer SUNFUNCS_RET_DOUBLE ] { 2 }
  }

  - Functions {
    Function [ <internal:date> function strtotime ] {

      - Parameters [2] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <optional> $now ]
      }
    }
    Function [ <internal:date> function date ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal:date> function idate ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal:date> function gmdate ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal:date> function mktime ] {

      - Parameters [6] {
        Parameter #0 [ <optional> $hour ]
        Parameter #1 [ <optional> $min ]
        Parameter #2 [ <optional> $sec ]
        Parameter #3 [ <optional> $mon ]
        Parameter #4 [ <optional> $day ]
        Parameter #5 [ <optional> $year ]
      }
    }
    Function [ <internal:date> function gmmktime ] {

      - Parameters [6] {
        Parameter #0 [ <optional> $hour ]
        Parameter #1 [ <optional> $min ]
        Parameter #2 [ <optional> $sec ]
        Parameter #3 [ <optional> $mon ]
        Parameter #4 [ <optional> $day ]
        Parameter #5 [ <optional> $year ]
      }
    }
    Function [ <internal:date> function checkdate ] {

      - Parameters [3] {
        Parameter #0 [ <required> $month ]
        Parameter #1 [ <required> $day ]
        Parameter #2 [ <required> $year ]
      }
    }
    Function [ <internal:date> function strftime ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal:date> function gmstrftime ] {

      - Parameters [2] {
        Parameter #0 [ <required> $format ]
        Parameter #1 [ <optional> $timestamp ]
      }
    }
    Function [ <internal:date> function time ] {

      - Parameters [0] {
      }
    }
    Function [ <internal:date> function localtime ] {

      - Parameters [2] {
        Parameter #0 [ <optional> $timestamp ]
        Parameter #1 [ <optional> $associative_array ]
      }
    }
    Function [ <internal:date> function getdate ] {

      - Parameters [1] {
        Parameter #0 [ <optional> $timestamp ]
      }
    }
    Function [ <internal:date> function date_create ] {
    }
    Function [ <internal:date> function date_create_from_format ] {
    }
    Function [ <internal:date> function date_parse ] {
    }
    Function [ <internal:date> function date_parse_from_format ] {
    }
    Function [ <internal:date> function date_get_last_errors ] {
    }
    Function [ <internal:date> function date_format ] {
    }
    Function [ <internal:date> function date_format_locale ] {
    }
    Function [ <internal:date> function date_modify ] {
    }
    Function [ <internal:date> function date_timezone_get ] {
    }
    Function [ <internal:date> function date_timezone_set ] {
    }
    Function [ <internal:date> function date_offset_get ] {
    }
    Function [ <internal:date> function date_time_set ] {
    }
    Function [ <internal:date> function date_date_set ] {
    }
    Function [ <internal:date> function date_isodate_set ] {
    }
    Function [ <internal:date> function date_timestamp_set ] {
    }
    Function [ <internal:date> function date_timestamp_get ] {
    }
    Function [ <internal:date> function timezone_open ] {
    }
    Function [ <internal:date> function timezone_name_get ] {
    }
    Function [ <internal:date> function timezone_name_from_abbr ] {
    }
    Function [ <internal:date> function timezone_offset_get ] {
    }
    Function [ <internal:date> function timezone_transitions_get ] {
    }
    Function [ <internal:date> function timezone_identifiers_list ] {
    }
    Function [ <internal:date> function timezone_abbreviations_list ] {
    }
    Function [ <internal:date> function date_default_timezone_set ] {

      - Parameters [1] {
        Parameter #0 [ <required> $timezone_identifier ]
      }
    }
    Function [ <internal:date> function date_default_timezone_get ] {

      - Parameters [0] {
      }
    }
    Function [ <internal:date> function date_sunrise ] {

      - Parameters [6] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <optional> $format ]
        Parameter #2 [ <optional> $latitude ]
        Parameter #3 [ <optional> $longitude ]
        Parameter #4 [ <optional> $zenith ]
        Parameter #5 [ <optional> $gmt_offset ]
      }
    }
    Function [ <internal:date> function date_sunset ] {

      - Parameters [6] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <optional> $format ]
        Parameter #2 [ <optional> $latitude ]
        Parameter #3 [ <optional> $longitude ]
        Parameter #4 [ <optional> $zenith ]
        Parameter #5 [ <optional> $gmt_offset ]
      }
    }
    Function [ <internal:date> function date_sun_info ] {

      - Parameters [3] {
        Parameter #0 [ <required> $time ]
        Parameter #1 [ <required> $latitude ]
        Parameter #2 [ <required> $longitude ]
      }
    }
  }

  - Classes [2] {
    Class [ <internal:date> class DateTime ] {

      - Constants [11] {
        Constant [ string ATOM ] { Y-m-d\TH:i:sP }
        Constant [ string COOKIE ] { l, d-M-y H:i:s T }
        Constant [ string ISO8601 ] { Y-m-d\TH:i:sO }
        Constant [ string RFC822 ] { D, d M y H:i:s O }
        Constant [ string RFC850 ] { l, d-M-y H:i:s T }
        Constant [ string RFC1036 ] { D, d M y H:i:s O }
        Constant [ string RFC1123 ] { D, d M Y H:i:s O }
        Constant [ string RFC2822 ] { D, d M Y H:i:s O }
        Constant [ string RFC3339 ] { Y-m-d\TH:i:sP }
        Constant [ string RSS ] { D, d M Y H:i:s O }
        Constant [ string W3C ] { Y-m-d\TH:i:sP }
      }

      - Static properties [0] {
      }

      - Static methods [2] {
        Method [ <internal:date> static public method createFromFormat ] {
        }

        Method [ <internal:date> static public method getLastErrors ] {
        }
      }

      - Properties [0] {
      }

      - Methods [11] {
        Method [ <internal:date, ctor> public method __construct ] {
        }

        Method [ <internal:date> public method format ] {
        }

        Method [ <internal:date> public method modify ] {
        }

        Method [ <internal:date> public method getTimezone ] {
        }

        Method [ <internal:date> public method setTimezone ] {
        }

        Method [ <internal:date> public method getOffset ] {
        }

        Method [ <internal:date> public method setTime ] {
        }

        Method [ <internal:date> public method setDate ] {
        }

        Method [ <internal:date> public method setISODate ] {
        }

        Method [ <internal:date> public method setTimestamp ] {
        }

        Method [ <internal:date> public method getTimestamp ] {
        }
      }
    }

    Class [ <internal:date> class DateTimeZone ] {

      - Constants [13] {
        Constant [ integer AFRICA ] { 1 }
        Constant [ integer AMERICA ] { 2 }
        Constant [ integer ANTARCTICA ] { 4 }
        Constant [ integer ARCTIC ] { 8 }
        Constant [ integer ASIA ] { 16 }
        Constant [ integer ATLANTIC ] { 32 }
        Constant [ integer AUSTRALIA ] { 64 }
        Constant [ integer EUROPE ] { 128 }
        Constant [ integer INDIAN ] { 256 }
        Constant [ integer PACIFIC ] { 512 }
        Constant [ integer UTC ] { 1024 }
        Constant [ integer ALL ] { 2047 }
        Constant [ integer ALL_WITH_BC ] { 4095 }
      }

      - Static properties [0] {
      }

      - Static methods [2] {
        Method [ <internal:date> static public method listAbbreviations ] {
        }

        Method [ <internal:date> static public method listIdentifiers ] {
        }
      }

      - Properties [0] {
      }

      - Methods [4] {
        Method [ <internal:date, ctor> public method __construct ] {
        }

        Method [ <internal:date> public method getName ] {
        }

        Method [ <internal:date> public method getOffset ] {
        }

        Method [ <internal:date> public method getTransitions ] {
        }
      }
    }
  }
}

"
Done
