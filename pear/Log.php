<?php
// $Id$
// $Horde: horde/lib/Log.php,v 1.15 2000/06/29 23:39:45 jon Exp $

/**
 * The Log:: class implements both an abstraction for various logging
 * mechanisms and the Subject end of a Subject-Observer pattern.
 *
 * @author  Chuck Hagenbuch <chuck@horde.org>
 * @author  Jon Parise <jon@csh.rit.edu>
 * @version $Revision$
 * @since   Horde 1.3
 */
class Log {

    // {{{ properties

    /** Boolean indicating whether or not the log connection is
        currently open. */
    var $opened = false;

    /** String holding the identifier that will be stored along with
        each logged message. */
    var $ident = '';

    /** Array holding all Log_observer objects that wish to be notified
        of any messages that we handle. */
    var $listeners = array();

    // }}}

    // {{{ factory()
    /**
     * Attempts to return a concrete Log instance of $log_type.
     * 
     * @param $log_type The type of concrete Log subclass to return.
     *                  Attempt to dynamically include the code for this
     *                  subclass. Currently, valid values are 'syslog',
     *                  'sql', 'file', and 'mcal'.
     *
     * @param $log_name (optional) The name of the actually log file,
     *                  table, or other specific store to use. Defaults
     *                  to an empty string, with which the subclass will
     *                  attempt to do something intelligent.
     *
     * @param $ident    (optional) The indentity reported to the log system.
     *
     * @param $conf     (optional) A hash containing any additional
     *                  configuration information that a subclass might need.
     * 
     * @return          The newly created concrete Log instance, or an
     *                  false on an error.
     */
    function &factory($log_type, $log_name = '', $ident = '', $conf = array())
    {
        $log_type = strtolower($log_type);
        $classfile = 'Log/' . $log_type . '.php';
        @include_once $classfile;
        $class = 'Log_' . $log_type;
        if (class_exists($class)) {
            return new $class($log_name, $ident, $conf);
        } else {
            return false;
        }
    }
    // }}}

    // {{{ singleton()
    /**
     * Attempts to return a reference to a concrete Log instance of
     * $log_type, only creating a new instance if no log instance with
     * the same parameters currently exists.
     *
     * You should use this if there are multiple places you might
     * create a logger, you don't want to create multiple loggers, and
     * you don't want to check for the existance of one each time. The
     * singleton pattern does all the checking work for you.
     *
     * <b>You MUST call this method with the $var = &Log::singleton()
     * syntax. Without the ampersand (&) in front of the method name,
     * you will not get a reference, you will get a copy.</b>
     * 
     * @param $log_type The type of concrete Log subclass to return.
     *                  Attempt to dynamically include the code for
     *                  this subclass. Currently, valid values are
     *                  'syslog', 'sql', 'file', and 'mcal'.
     *
     * @param $log_name (optional) The name of the actually log file,
     *                  table, or other specific store to use.  Defaults
     *                  to an empty string, with which the subclass will
     *                  attempt to do something intelligent.
     *
     * @param $ident    (optional) The identity reported to the log system.
     *
     * @param $conf     (optional) A hash containing any additional
     *                  configuration information that a subclass might need.
     * 
     * @return          The concrete Log reference, or false on an error.
     */
    function &singleton ($log_type, $log_name = '', $ident = '', $conf = array()) {
        static $instances;
        if (!isset($instances)) $instances = array();
        
        $signature = md5($log_type . '][' . $log_name . '][' . $ident . '][' . implode('][', $conf));
        if (!isset($instances[$signature])) {
            $instances[$signature] = &Log::factory($log_type, $log_name, $ident, $conf);
        }
        return $instances[$signature];
    }
    // }}}

    // {{{ priorityToString()
    /**
     * Returns the string representation of a LOG_* integer constant.
     *
     * @param $priority The LOG_* integer constant.
     *
     * @return          The string representation of $priority.
     */
    function priorityToString ($priority) {
        $priorities = array(
            LOG_EMERG   => 'emergency',
            LOG_ALERT   => 'alert',
            LOG_CRIT    => 'critical',
            LOG_ERR     => 'error',
            LOG_WARNING => 'warning',
            LOG_NOTICE  => 'notice',
            LOG_INFO    => 'info',
            LOG_DEBUG   => 'debug'
        );
        return $priorities[$priority];
    }
    // }}}

    // {{{ attach()
    /**
     * Adds a Log_observer instance to the list of observers that are
     * be notified when a message is logged.
     *  
     * @param $logObserver  The Log_observer instance to be added to
     *                      the $listeners array.
     */
    function attach (&$logObserver) {
        if (!is_object($logObserver))
            return false;
        
        $logObserver->_listenerID = uniqid(rand());
        
        $this->listeners[$logObserver->_listenerID] = &$logObserver;
    }
    // }}}

    // {{{ detach()
    /**
     * Removes a Log_observer instance from the list of observers.
     *
     * @param $logObserver  The Log_observer instance to be removed
     *                      from the $listeners array.
     */
    function detach ($logObserver) {
        if (isset($this->listeners[$logObserver->_listenerID]))
            unset($this->listeners[$logObserver->_listenerID]);
    }
    // }}}

    // {{{ notifyAll()
    /**
     * Sends any Log_observer objects listening to this Log the message
     * that was just logged.
     *
     * @param $messageOb    The data structure holding all relevant log
     *                      information - the message, the priority, what
     *                      log this is, etc.
     */
    function notifyAll ($messageOb) {
        reset($this->listeners);
        foreach ($this->listeners as $listener) {
            if ($messageOb['priority'] <= $listener->priority)
                $listener->notify($messageOb);
        }
    }
    // }}}

    // {{{ isComposite()
    /**
     * @return a Boolean: true if this is a composite class, false
     * otherwise. The composite subclass overrides this to return
     * true.
     */
    function isComposite () {
        return false;
    }
    // }}}
}

?>
