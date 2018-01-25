--TEST--
final public int ErrorException::getSeverity ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--INI--
error_reporting = E_ALL
--FILE--
<?php

const EXCEPTION_CODE_ERROR = 0;
const EXCEPTION_SEVERITY_ERROR_MSG = "This exception severity is: ";
const EXCEPTION_PARAM_MSG = "string message param";
const EXCEPTION_TRACE_AS_STRING_MSG = "#0 {main}";

try {
    throw new ErrorException();
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_ERROR);
    var_dump($e->getMessage() === "");
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_ERROR);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_WARNING);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_PARSE);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_PARSE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_NOTICE);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_NOTICE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_ERROR);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_ERROR);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_COMPILE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_WARNING);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_COMPILE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_ERROR);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_WARNING);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_NOTICE);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_NOTICE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_STRICT);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_STRICT);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_RECOVERABLE_ERROR);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_RECOVERABLE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_DEPRECATED);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_DEPRECATED);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_ERROR, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_WARNING, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_PARSE, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_PARSE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_NOTICE, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_NOTICE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_ERROR, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_ERROR, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_COMPILE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_WARNING, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_COMPILE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_ERROR, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_WARNING, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_NOTICE, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_NOTICE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_STRICT, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_STRICT);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_RECOVERABLE_ERROR, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_RECOVERABLE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_DEPRECATED, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_DEPRECATED, __FILE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_ERROR, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_WARNING, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_PARSE, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_PARSE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_NOTICE, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_NOTICE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_ERROR, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_CORE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_ERROR, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_COMPILE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_WARNING, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_COMPILE_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_ERROR, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_WARNING, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_WARNING);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_NOTICE, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_NOTICE);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_STRICT, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_STRICT);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_RECOVERABLE_ERROR, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_RECOVERABLE_ERROR);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_DEPRECATED, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_DEPRECATED, __FILE__, __LINE__);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_DEPRECATED, __FILE__, __LINE__, NULL);
} catch(ErrorException $e) {
    echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
    var_dump($e->getSeverity() === E_USER_DEPRECATED);
    var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
    var_dump($e->getCode() === 0);
    var_dump($e->getPrevious() === NULL);
    var_dump($e->getFile() === __FILE__);
    var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_ERROR, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_ERROR, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_ERROR);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_WARNING, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_WARNING, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_WARNING);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_PARSE, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_PARSE, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_PARSE);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}


try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_NOTICE, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_NOTICE, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_NOTICE);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_ERROR, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_ERROR, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_CORE_ERROR);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_CORE_WARNING, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_CORE_WARNING);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_ERROR, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_ERROR, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_COMPILE_ERROR);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_WARNING, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_COMPILE_WARNING, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_COMPILE_WARNING);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_ERROR, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_ERROR, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_USER_ERROR);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_WARNING, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_WARNING, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_USER_WARNING);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_NOTICE, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_NOTICE, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_USER_NOTICE);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_STRICT, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_STRICT, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_STRICT);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_RECOVERABLE_ERROR, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_RECOVERABLE_ERROR, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_RECOVERABLE_ERROR);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

try {
    throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_DEPRECATED, __FILE__, __LINE__, NULL);
} catch(Exception $exceptionErr) {
    try {
        throw new ErrorException(EXCEPTION_PARAM_MSG, EXCEPTION_CODE_ERROR, E_USER_DEPRECATED, __FILE__, __LINE__, $exceptionErr->getPrevious());
    } catch(ErrorException $e) {
        echo EXCEPTION_SEVERITY_ERROR_MSG . $e->getSeverity();
        var_dump($e->getSeverity() === E_USER_DEPRECATED);
        var_dump($e->getMessage() === EXCEPTION_PARAM_MSG);
        var_dump($e->getCode() === 0);
        var_dump($e->getPrevious() === NULL);
        var_dump($e->getFile() === __FILE__);
        var_dump($e->getTraceAsString() === EXCEPTION_TRACE_AS_STRING_MSG);
    }
}

?>
--EXPECTF--
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 64bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 128bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 256bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 512bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1024bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2048bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4096bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8192bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16384bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 64bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 128bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 256bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 512bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1024bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2048bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4096bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8192bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16384bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 64bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 128bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 256bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 512bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1024bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2048bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4096bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8192bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16384bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16384bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 8bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 32bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 64bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 128bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 256bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 512bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 1024bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 2048bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 4096bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
This exception severity is: 16384bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
