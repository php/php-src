<?php
// +----------------------------------------------------------------------+
// | PHP version 4.0                                                      |
// +----------------------------------------------------------------------+
// | Copyright (c) 1997, 1998, 1999, 2000, 2001 The PHP Group             |
// +----------------------------------------------------------------------+
// | This source file is subject to version 2.0 of the PHP license,       |
// | that is bundled with this package in the file LICENSE, and is        |
// | available at through the world-wide-web at                           |
// | http://www.php.net/license/2_02.txt.                                 |
// | If you did not receive a copy of the PHP license and are unable to   |
// | obtain it through the world-wide-web, please send a note to          |
// | license@php.net so we can mail you a copy immediately.               |
// +----------------------------------------------------------------------+
// | Authors: Ulf Wendel <ulf.wendel@phpdoc.de>                           |
// |          Sebastian Bergmann <sb@sebastian-bergmann.de>               |
// +----------------------------------------------------------------------+
//
// $Id$

/**
* Cache is a base class for cache implementations.
*
* TODO: Simple usage example goes here.
*
* @author   Ulf Wendel <ulf.wendel@phpdoc.de>
* @version  $Id$
* @package  Cache
* @access   public 
*/
class Cache {

    /**
    * Disables the caching.
    *
    * TODO: Add explanation what this is good for.
    *
    * @var      boolean
    * @access   public
    */
    var $no_cache = false;

    /**
    * Garbage collection: probability in seconds
    *
    * If set to a value above 0 a garbage collection will 
    * flush all cache entries older than the specified number 
    * of seconds.
    *
    * @var      integer
    * @see      $gc_probability
    * @access   public
    */
    var $gc_time  = 1;

    /**
    * Garbage collection: probability in percent
    *
    * TODO: Add an explanation.
    *
    * @var      integer     0 => never
    * @see      $gc_time
    * @access   public
    */
    var $gc_probability = 1;
    
    /**
    * Storage container object.
    * 
    * @var  object Cache_Container
    */    
    var $container;

    //
    // public methods
    //

    /**    
    *
    * @param    string  Name of storage container class
    * @param    array   Array with storage class dependend config options
    * @see      setOptions()
    */
    function Cache($storage_driver, $storage_options = "")
    {
        $storage_driver = strtolower($storage_driver);
        $storage_class = 'Cache_Container_' . $storage_driver;
	$storage_classfile = 'Cache/Container/' . $storage_driver . '.php';
	
        if (@include_once $storage_classfile) {
            $this->container = new $storage_class($storage_options);
            $this->garbageCollection();
        } else {
            return null;
        }
    }
    
    /**
    * Returns the requested dataset it if exists and is not expired
    *  
    * @param    string  dataset ID
    * @return   mixed   cached data or NULL on failure
    * @access   public
    */
    function get($id) {
        if ($this->no_cache)
            return "";
            
        if ($this->isCached($id) && !$this->isExpired($id))
            return $this->load($id);
        
        return NULL;            
    } // end func get

    /**
    * Stores the given data in the cache.
    * 
    * @param    string  dataset ID used as cache identifier
    * @param    mixed   data to cache
    * @param    integer lifetime of the cached data in seconds - 0 for endless
    * @return   boolean
    * @access   public
    */
    function save($id, $data, $expires = 0) {
        if ($this->no_cache)
            return true;
            
        return $this->container->save($id, $data, $expires);
    } // end func save
    
    /**
    * Loads the given ID from the cache.
    * 
    * @param    string  dataset ID
    * @return   mixed   cached data or NULL on failure 
    * @access   public
    */
    function load($id) {
        if ($this->no_cache)
            return "";
            
        return $this->container->load($id);
    } // end func load
    
    /**
    * Removes the specified dataset from the cache.
    * 
    * @param    string  dataset ID
    * @return   boolean
    * @access   public
    */
    function delete($id) {
        if ($this->no_cache)
            return true;
            
        return $this->container->delete($id);
    } // end func delete
    
    /**
    * Flushes the cache - removes all data from it
    * 
    * @return   integer number of removed datasets
    */
    function flush() {
        if ($this->no_cache)
            return true;
            
        return $this->container->flush();
    } // end func flush
    
    /**
    * Checks if a dataset exists.
    * 
    * Note: this does not say that the cached data is not expired!
    * 
    * @param    string  dataset ID
    * @return   boolean
    * @access   public
    */
    function isCached($id) {
        if ($this->no_cache)
            return false;
            
        return $this->container->isCached($id);
    } // end func isCached
    
    /**
    * Checks if a dataset is expired
    * 
    * @param    string  dataset ID
    * @param    integer maximum age for the cached data in seconds - 0 for endless
    *                   If the cached data is older but the given lifetime it will
    *                   be removed from the cache. You don't have to provide this 
    *                   argument if you call isExpired(). Every dataset knows 
    *                   it's expire date and will be removed automatically. Use 
    *                   this only if you know what you're doing...
    * @return   boolean
    * @access   public
    */
    function isExpired($id, $max_age = 0) {
        if ($this->no_cache)
            return true;
            
        return $this->container->isExpired($id, $max_age);
    } // end func isExpired
    
    /**
    * Generates a "unique" ID for the given value
    * 
    * This is a quick but dirty hack to get a "unique" ID for a any kind of variable.
    * ID clashes might occur from time to time although they are extreme unlikely!
    *
    * @param    mixed   variable to generate a ID for
    * @return   string  "unique" ID
    * @access   public
    */
    function generateID($variable) {
        // WARNING: ID clashes are possible although unlikely
        return md5(serialize($variable));
    }

    /**
    * Calls the garbage collector of the storage object with a certain probability
    * 
    * @param    boolean Force a garbage collection run?
    * @see  $gc_probability, $gc_time, setOptions()
    */
    function garbageCollection($force = false) {
        static $last_run = 0;
        
        if ($this->no_cache)
            return;

        srand((double) microtime() * 1000000);
        
        // time and probability based
        if (($force) || ($last_run && $last_run < time() + $this->gc_time) || (rand(1, 100) < $this->gc_probability)) {
            $this->container->garbageCollection();
            $last_run = time();
        }
        
    } // end func garbageCollection

    
} // end class cache 
?>
