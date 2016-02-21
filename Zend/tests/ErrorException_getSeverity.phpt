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
           $err->getTraceAsString()
        );
        print(is_array($err->getTrace()) ? ("array();") : ("method fail"));
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
string(%d) "%s"
array();
