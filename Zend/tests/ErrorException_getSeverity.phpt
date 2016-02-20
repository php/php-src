--TEST--
final public int ErrorException::getSeverity ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
function exception_error_handler($severity) {
 
    $message = "error exception, severity =>";
    $code = 500;
    $severity = 75;
    try{
        if (!(error_reporting() & $severity)) {
            return;
        }
        throw new ErrorException($message, $code, $severity);
    } catch(ErrorException $err) {
        var_dump(
           $err->getMessage(), 
           $err->getSeverity(), 
           $err->getCode(), 
           $err->getPrevious(), 
           $err->getFile(), 
           $err->getLine(), 
           $err->getTrace(),
           $err->getTraceAsString()
        );  
    }
    
}
set_error_handler("exception_error_handler");
/* Trigger exception */
preg_match();
?>
--EXPECTF--
string(%d) "%s"
int(%d)
int(%d)
NULL
string(%d) "%s"
int(%d)
array(%d) {
  [%i]=>
  array(%d) {
    ["function"]=>
    string(%d) "%s"
    ["args"]=>
    array(%d) {
      [%d]=>
      &int(%d)
      [%d]=>
      &string(%d) "%s"
      [%d]=>
      &string(%d) "%s"
      [%d]=>
      &int(%d)
      [%d]=>
      &array(%d) {
        ["_GET"]=>
        array(%i) {
        }
        ["_POST"]=>
        array(%i) {
        }
        ["_COOKIE"]=>
        array(%i) {
        }
        ["_FILES"]=>
        array(%i) {
        }
        ["argv"]=>
        array(%d) {
          [%i]=>
          string(%d) "%s"
        }
        ["argc"]=>
        int(%d)
        ["_ENV"]=>
        array(%i) {
        }
        ["_REQUEST"]=>
        array(%i) {
        }
        ["_SERVER"]=>
        array(%d) {
          ["SCRIPT_FILENAME"]=>
          string(%d) "%s"
          ["PWD"]=>
          string(%d) "%s"
          ["REDIRECT_STATUS"]=>
          string(%d) "%s"
          ["PATH_TRANSLATED"]=>
          string(%d) "%s"
          ["SHLVL"]=>
          string(%d) "%s"
          ["REQUEST_METHOD"]=>
          string(%d) "GET"
          ["_"]=>
          string(%d) "%s"
          ["PHP_SELF"]=>
          string(%d) "%s"
          ["SCRIPT_NAME"]=>
          string(%d) "%s"
          ["DOCUMENT_ROOT"]=>
          string(%i) ""
          ["REQUEST_TIME_FLOAT"]=>
          float(%f)
          ["REQUEST_TIME"]=>
          int(%i)
          ["argv"]=>
          array(%d) {
            [%i]=>
            string(%d) "%s"
          }
          ["argc"]=>
          int(%d)
        }
      }
    }
  }
  [%d]=>
  array(%d) {
    ["file"]=>
    string(%d) "%s"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(%d) "%s"
    ["args"]=>
    array(%i) {
    }
  }
}
string(%d) "%s"
