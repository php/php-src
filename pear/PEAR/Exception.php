<?php
/* vim: set expandtab tabstop=4 shiftwidth=4 foldmethod=marker: */
// +----------------------------------------------------------------------+
// | PEAR_Exception                                                       |
// +----------------------------------------------------------------------+
// | Copyright (c) 2004 The PEAR Group                                    |
// +----------------------------------------------------------------------+
// | This source file is subject to version 3.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/3_0.txt.                                  |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Tomas V.V.Cox <cox@idecnet.com>                             |
// |          Hans Lellelid <hans@velum.net>                              |
// +----------------------------------------------------------------------+
//
// $Id$

define('PEAR_OBSERVER_PRINT',      -2);
define('PEAR_OBSERVER_TRIGGER',    -4);
define('PEAR_OBSERVER_DIE',        -8);

/**
 * Base PEAR_Exception Class
 *
 * Features:
 *
 * - Nestable exceptions (throw new PEAR_Exception($msg, $prev_exception))
 * - Definable triggers, shot when exceptions occur
 * - Pretty and informative error messages
 * - Added more context info avaible (like class, method or cause)
 *
 * Ideas:
 *
 * - Maybe a way to define a 'template' for the output
 *
 * 1) Inherited properties from PHP Exception Class:
 *
 * protected $message
 * protected $code
 * protected $line
 * protected $file
 * private   $trace
 *
 * 2) Inherited methods from PHP Exception Class:
 *
 * __clone
 * __construct
 * getMessage
 * getCode
 * getFile
 * getLine
 * getTrace
 * getTraceAsString
 * __toString
 *
 * 3) Usage example
 *
 * <code>
 *  require_once 'PEAR/Exception.php';
 *
 *  class Test {
 *     function foo() {
 *         throw new PEAR_Exception('Error Message', ERROR_CODE);
 *     }
 *  }
 *
 *  function myLogger($pear_exception) {
 *     echo $pear_exception->getMessage();
 *  }
 *  // each time a exception is thrown the 'myLogger' will be called
 *  // (its use is completely optional)
 *  PEAR_Exception::addObserver('mylogger', 'myLogger');
 *  $test = new Test;
 *  try {
 *     $test->foo();
 *  } catch (PEAR_Exception $e) {
 *     print $e;
 *  }
 * </code>
 *
 * @since PHP 5
 * @package PEAR
 * @version $Rev:$
 * @author Tomas V.V.Cox <cox@idecnet.com>
 * @author Hans Lellelid <hans@velum.net>
 *
 */
class PEAR_Exception extends Exception
{
    protected $cause;
    protected $error_class;
    protected $error_method;

    private $method;
    private static $_observers = array();

    /**
     * Supported signatures:
     * PEAR_Exception(string $message);
     * PEAR_Exception(string $message, int $code);
     * PEAR_Exception(string $message, Exception $cause);
     * PEAR_Exception(string $message, Exception $cause, int $code);
     */
    public function __construct($message, $p2 = null, $p3 = null)
    {
        $code = null;
        $cause = null;
        if (is_int($p3) && $p2 instanceof Exception) {
            $code = $p3;
            $cause = $p2;
        } elseif (is_int($p2)) {
            $code = $p2;
        } elseif ($p2 instanceof Exception) {
            $cause = $p2;
        }
        $this->cause = $cause;
        $trace       = parent::getTrace();
        $this->error_class  = $trace[0]['class'];
        $this->error_method = $trace[0]['function'];
        $this->method = $this->error_class.'::'.$this->error_method.'()';
        parent::__construct($message, $code);

        $this->signal();
    }

    public static function addObserver($label, $callback)
    {
        self::$_observers[$label] = $callback;
    }

    public static function delObserver($label)
    {
        unset(self::$_observers[$label]);
    }

    private function signal()
    {
        foreach (self::$_observers as $func) {
            if (is_callable($func)) {
                call_user_func($func, $this);
                continue;
            }
            settype($func, 'array');
            switch ($func[0]) {
                case PEAR_OBSERVER_PRINT:
                    $f = (isset($func[1])) ? $func[1] : '%s';
                    printf($f, $this->getMessage());
                    break;
                case PEAR_OBSERVER_TRIGGER:
                    $f = (isset($func[1])) ? $func[1] : E_USER_NOTICE;
                    trigger_error($this->getMessage(), $f);
                    break;
                case PEAR_OBSERVER_DIE:
                    $f = (isset($func[1])) ? $func[1] : '%s';
                    die(printf($f, $this->getMessage()));
                    break;
                default:
                    trigger_error('invalid observer type', E_USER_WARNING);
            }
        }
    }

    public function getCauseMessage($obj)
    {
        $msg = '     ' . $obj->method . " at {$obj->file} ({$obj->line})\n";
        if ($obj->cause instanceof Exception) {
            return $msg.$obj->getCauseMessage($obj->cause);
        }
        return $msg;
    }

    /**
     * @return Exception_object The context of the exception
     */
    public function getCause()
    {
        return $this->cause;
    }

    public function getErrorClass()
    {
        return $this->error_class;
    }

    public function getErrorMethod()
    {
        return $this->error_method;
    }

    public function __toString()
    {
        $str = get_class($this) . " occurred: \n" .
               '  Error message: ' . $this->message . "\n" .
               '  Error code   : ' . $this->code . "\n" .
               '  File (Line)  : ' . "{$this->file} ({$this->line})\n" .
               '  Method       : ' . $this->method . "\n";
        if ($this->cause instanceof Exception) {
            $str .= "  Nested Error : \n".$this->getCauseMessage($this);
        }
        if (isset($_SERVER['REQUEST_URI'])) {
            return nl2br('<pre>'.htmlentities($str).'</pre>');
        }
        return $str;
    }
}

?>